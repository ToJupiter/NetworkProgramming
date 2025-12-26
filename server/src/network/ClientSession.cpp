#include "ClientSession.h"
#include "Server.h"
#include "../db/DatabaseManager.h"
#include "../db/UserRepository.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

ClientSession::ClientSession(int fd, Server* server) 
    : clientFd(fd), server(server), markedForDeletion(false) {
    recvBuffer.reserve(4096);
    sendBuffer.reserve(4096);
}

ClientSession::~ClientSession() {
    if (clientFd >= 0) close(clientFd);
}

void ClientSession::onDisconnect() {
    if (state.currentRoomId != 0) {
        server->getRoomManager()->leaveRoom(state.currentRoomId, state.userId);
        state.currentRoomId = 0;
    }
}


void ClientSession::readData() {
    uint8_t tempBuf[4096];
    ssize_t bytes = read(clientFd, tempBuf, sizeof(tempBuf));

    if (bytes > 0) {
        recvBuffer.insert(recvBuffer.end(), tempBuf, tempBuf + bytes);
        processBuffer();
    } else if (bytes == 0 || (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        markedForDeletion = true;
    }
}

void ClientSession::processBuffer() {
    while (recvBuffer.size() >= sizeof(MessageHeader)) {
        MessageHeader header;
        std::memcpy(&header, recvBuffer.data(), sizeof(MessageHeader));


        header.body_len = ntohl(header.body_len);
        
        if (recvBuffer.size() < sizeof(MessageHeader) + header.body_len) {
            break; 
        }

        std::vector<uint8_t> body(
            recvBuffer.begin() + sizeof(MessageHeader),
            recvBuffer.begin() + sizeof(MessageHeader) + header.body_len
        );

        handleMessage(header, body);

        recvBuffer.erase(
            recvBuffer.begin(), 
            recvBuffer.begin() + sizeof(MessageHeader) + header.body_len
        );
    }
}

void ClientSession::handleMessage(const MessageHeader& header, const std::vector<uint8_t>& body) {
    switch (header.type) {
        case MessageType::C2S_REGISTER_REQ: {
            if (body.size() >= sizeof(RegisterRequest)) {
                handleRegister(reinterpret_cast<const RegisterRequest*>(body.data()));
            }
            break;
        }
        case MessageType::C2S_LOGIN_REQ: {
            if (body.size() >= sizeof(LoginRequest)) {
                handleLogin(reinterpret_cast<const LoginRequest*>(body.data()));
            }
            break;
        }
        case MessageType::C2S_CREATE_ROOM_REQ:
            if (state.isAuthenticated && body.size() >= sizeof(CreateRoomRequest)) 
                handleCreateRoom((const CreateRoomRequest*)body.data());
            break;
        case MessageType::C2S_LIST_ROOMS_REQ:
            if (state.isAuthenticated) handleListRooms();
            break;
        case MessageType::C2S_JOIN_ROOM_REQ:
            if (state.isAuthenticated && body.size() >= sizeof(JoinRoomRequest)) 
                handleJoinRoom((const JoinRoomRequest*)body.data());
            break;
        case MessageType::C2S_LEAVE_ROOM_REQ:
            if (state.isAuthenticated) handleLeaveRoom();
            break;
        case MessageType::C2S_READY_STATUS_REQ:
            if (state.isAuthenticated && body.size() >= sizeof(ReadyStatusRequest))
                handleReadyStatus((const ReadyStatusRequest*)body.data());
            break;
        case MessageType::C2S_SUBMIT_ANSWER_REQ:
            if (state.isAuthenticated && body.size() >= sizeof(SubmitAnswerRequest))
                handleSubmitAnswer(reinterpret_cast<const SubmitAnswerRequest*>(body.data()));
            break;
        case MessageType::C2S_GET_STATS_REQ:
            if (state.isAuthenticated) handleGetStats();
            break;
        case MessageType::C2S_PAUSE_GAME_REQ:
            if (state.isAuthenticated) handlePauseGame();
            break;
        case MessageType::C2S_RESUME_GAME_REQ:
            if (state.isAuthenticated) handleResumeGame();
            break;
        default:
            break;
    }
}

void ClientSession::handleRegister(const RegisterRequest* req) {
    std::string email(req->email);
    std::string display(req->display_name);
    std::string pass(req->password);

    StatusCode status = DatabaseManager::getInstance().registerUser(email, display, pass);
    
    StatusResponse rsp;
    rsp.code = status;
    sendResponse(MessageType::S2C_REGISTER_RSP, &rsp, sizeof(rsp));
}

void ClientSession::handleLogin(const LoginRequest* req) {
    std::string email(req->email);
    std::string pass(req->password);

    auto user = DatabaseManager::getInstance().loginUser(email, pass);

    if (user.has_value()) {
        state.isAuthenticated = true;
        state.userId = user->id;
        state.displayName = user->display_name;

        LoginResponse rsp;
        rsp.code = StatusCode::SUCCESS;
        rsp.user_id = user->id;
        std::strncpy(rsp.display_name, user->display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
        sendResponse(MessageType::S2C_LOGIN_RSP, &rsp, sizeof(rsp));
    } else {
        LoginResponse rsp;
        rsp.code = StatusCode::INVALID_CREDENTIALS;
        rsp.user_id = 0;
        std::memset(rsp.display_name, 0, MAX_DISPLAY_NAME_LEN);
        sendResponse(MessageType::S2C_LOGIN_RSP, &rsp, sizeof(rsp));
    }
}

void ClientSession::handleCreateRoom(const CreateRoomRequest* req){
    if (state.currentRoomId != 0){
        CreateRoomResponse rsp;
        rsp.code = StatusCode::FAILURE_GENERIC;
        sendResponse(MessageType::S2C_CREATE_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    Room* room = server->getRoomManager()->createRoom(state.userId, *req);
    if (room) {
        state.currentRoomId = room->getId();
        room->addPlayer(this);
        
        CreateRoomResponse rsp;
        rsp.code = StatusCode::SUCCESS;
        rsp.room_info = room->getRoomInfo();
        sendResponse(MessageType::S2C_CREATE_ROOM_RSP, &rsp, sizeof(rsp));
    }
}

void ClientSession::handleListRooms() {
    ListRoomsResponse rsp;
    server->getRoomManager()->getAllRooms(rsp);
    sendResponse(MessageType::S2C_LIST_ROOMS_RSP, &rsp, sizeof(rsp));
}

void ClientSession::handleJoinRoom(const JoinRoomRequest* req) {
    if (state.currentRoomId != 0) {
        // If the client requests the room they are already in, return success with current data
        if (req->room_id == state.currentRoomId) {
            Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
            if (room) {
                JoinRoomResponse rsp{};
                rsp.code = StatusCode::SUCCESS;
                rsp.room_info = room->getRoomInfo();
                rsp.host_user_id = room->getHostId();
                room->getPlayerList(rsp);
                sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
                return;
            }
        }
        // Otherwise reject cross-room joins
        JoinRoomResponse rsp{};
        rsp.code = StatusCode::FAILURE_GENERIC; 
        rsp.room_info.room_id = state.currentRoomId;
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    Room* room = server->getRoomManager()->getRoom(req->room_id);
    if (!room) {
        JoinRoomResponse rsp{};
        rsp.code = StatusCode::ROOM_NOT_FOUND;
        rsp.room_info.room_id = req->room_id;
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    if (room->isFull()) {
        JoinRoomResponse rsp{};
        rsp.code = StatusCode::ROOM_FULL;
        rsp.room_info = room->getRoomInfo();
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    if (room->addPlayer(this)) {
        state.currentRoomId = room->getId();
        JoinRoomResponse rsp;
        rsp.code = StatusCode::SUCCESS;
        rsp.room_info = room->getRoomInfo();
        rsp.host_user_id = room->getHostId();
        room->getPlayerList(rsp);
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
    } else {
        JoinRoomResponse rsp{};
        rsp.code = StatusCode::GAME_IN_PROGRESS;
        rsp.room_info = room->getRoomInfo();
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
    }
}

void ClientSession::handleLeaveRoom() {
    if (state.currentRoomId == 0) return;
    
    server->getRoomManager()->leaveRoom(state.currentRoomId, state.userId);
    state.currentRoomId = 0;
    
}

void ClientSession::handleReadyStatus(const ReadyStatusRequest* req) {
    if (state.currentRoomId == 0) return;

    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->setPlayerReady(state.userId, req->is_ready);
    }
}

void ClientSession::sendResponse(MessageType type, const void* data, uint32_t len) {
    MessageHeader header;
    header.type = type;
    header.body_len = htonl(len);

    const uint8_t* hdrPtr = reinterpret_cast<const uint8_t*>(&header);
    sendBuffer.insert(sendBuffer.end(), hdrPtr, hdrPtr + sizeof(header));

    if (len > 0 && data != nullptr) {
        const uint8_t* bodyPtr = reinterpret_cast<const uint8_t*>(data);
        sendBuffer.insert(sendBuffer.end(), bodyPtr, bodyPtr + len);
    }
}

bool ClientSession::wantWrite() const {
    return !sendBuffer.empty();
}

void ClientSession::writeData() {
    if (sendBuffer.empty()) return;

    ssize_t sent = write(clientFd, sendBuffer.data(), sendBuffer.size());
    if (sent > 0) {
        sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + sent);
    } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        markedForDeletion = true;
    }
}

void ClientSession::sendMsg(MessageType type, const void* data, uint32_t len) {
    sendResponse(type, data, len);
}

void ClientSession::handleSubmitAnswer(const SubmitAnswerRequest* req) {
    if (state.currentRoomId == 0) return;

    Room *room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->handleSubmitAnswer(state.userId, *req);
    }
}

void ClientSession::handleGetStats() {
    UserStatsResponse stats = UserRepository::getUserStats(state.userId);
    sendResponse(MessageType::S2C_GET_STATS_RSP, &stats, sizeof(stats));
}

void ClientSession::handlePauseGame() {
    if (state.currentRoomId == 0) return;
    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->handlePauseGame(state.userId);
    }
}

void ClientSession::handleResumeGame() {
    if (state.currentRoomId == 0) return;
    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->handleResumeGame(state.userId);
    }
}