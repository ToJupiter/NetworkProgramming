# Source Code Collection

## server/CMakeLists.txt
``` txt
cmake_minimum_required(VERSION 3.10.0)
project(Server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SERVER_SOURCES 
    src/main.cpp
    src/db/DatabaseManager.cpp
    src/network/Server.cpp
    src/network/ClientSession.cpp
    src/game/Room.cpp
    src/game/RoomManager.cpp
)

### Third-party library handler ###
# Build sqlite3 as static lib
add_library(sqlite3 STATIC thirdparty/sqlite3/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite3)
set_target_properties(sqlite3 PROPERTIES LINKER_LANGUAGE C)

# header only sqlite_modern
add_library(sqlite_modern_cpp INTERFACE)
target_include_directories(sqlite_modern_cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite_modern_cpp/hdr)
target_link_libraries(sqlite_modern_cpp INTERFACE sqlite3)

# bcrypt library
add_subdirectory(thirdparty/libbcrypt)
set(THIRDPARTY_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite3
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite_modern_cpp/hdr
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libbcrypt
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

### Third-party library handler ends here ###

# Link main.cpp with libaries
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE sqlite3 sqlite_modern_cpp bcrypt pthread dl)
target_include_directories(server PRIVATE ${THIRDPARTY_SOURCES})

```

## common/protocol.h
``` h
#pragma once

#include <cstdint>

#define MAX_EMAIL_LEN 64
#define MAX_PASSWORD_LEN 64
#define MAX_DISPLAY_NAME_LEN 32
#define MAX_ROOM_NAME_LEN 32
#define MAX_QUESTION_CONTENT_LEN 256
#define MAX_OPTION_CONTENT_LEN 128
#define MAX_ROOMS_PER_LIST 50
#define MAX_PLAYERS_PER_ROOM 10
#define MAX_ERROR_MSG_LEN 128

enum class MessageType : uint16_t {
    C2S_REGISTER_REQ,
    S2C_REGISTER_RSP,
    C2S_LOGIN_REQ,
    S2C_LOGIN_RSP,
    C2S_ACCOUNT_UPDATE_REQ,
    S2C_ACCOUNT_UPDATE_RSP,

    C2S_LIST_ROOMS_REQ,
    S2C_LIST_ROOMS_RSP,
    C2S_CREATE_ROOM_REQ,
    S2C_CREATE_ROOM_RSP,
    C2S_JOIN_ROOM_REQ,
    S2C_JOIN_ROOM_RSP,
    S2C_PLAYER_JOINED_NOTIF,
    C2S_LEAVE_ROOM_REQ,
    S2C_PLAYER_LEFT_NOTIF,
    C2S_READY_STATUS_REQ,
    S2C_READY_STATUS_NOTIF,

    S2C_GAME_START_NOTIF,
    S2C_QUESTION_NOTIF,
    C2S_SUBMIT_ANSWER_REQ,
    S2C_ROUND_RESULT_NOTIF,
    S2C_PLAYER_ELIMINATED_NOTIF,
    S2C_GAME_OVER_NOTIF,
    C2S_LEAVE_MATCH_REQ,

    C2S_GET_STATS_REQ,
    S2C_GET_STATS_RSP,

    S2C_ERROR_RSP
};

enum class StatusCode : uint8_t {
    SUCCESS,
    FAILURE_GENERIC,
    EMAIL_EXISTS,
    INVALID_CREDENTIALS,
    ROOM_FULL,
    ROOM_NOT_FOUND,
    GAME_IN_PROGRESS,
    INVALID_REQUEST
};

enum class GameMode : uint8_t {
    ELIMINATION,
    SCORING
};

#pragma pack(push, 1)

struct MessageHeader {
    MessageType type;
    uint32_t body_len;
};

struct RegisterRequest {
    char email[MAX_EMAIL_LEN];
    char display_name[MAX_DISPLAY_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
};

struct LoginRequest {
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
};

struct AccountUpdateRequest {
    char new_display_name[MAX_DISPLAY_NAME_LEN];
    // may add more options to change other stats
};

struct StatusResponse {
    StatusCode code;
};

struct LoginResponse {
    StatusCode code;
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
};

struct RoomInfo {
    uint32_t room_id;
    char room_name[MAX_ROOM_NAME_LEN];
    uint8_t current_players;
    uint8_t max_players;
    GameMode game_mode;
    bool is_in_game;
};

struct ListRoomsResponse {
    uint16_t room_count;
    RoomInfo rooms[MAX_ROOMS_PER_LIST];
};

struct CreateRoomRequest {
    char room_name[MAX_ROOM_NAME_LEN];
    GameMode game_mode;
    uint8_t num_questions;
};

struct CreateRoomResponse {
    StatusCode code;
    RoomInfo room_info;
};

struct JoinRoomRequest {
    uint32_t room_id;
};

struct PlayerInfo {
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
    bool is_ready;
};

struct JoinRoomResponse {
    StatusCode code;
    RoomInfo room_info;
    uint8_t player_count;
    PlayerInfo players[MAX_PLAYERS_PER_ROOM];
    uint32_t host_user_id;
};

struct PlayerLeftNotification {
    uint32_t user_id;
    uint32_t new_host_user_id;
};

struct ReadyStatusRequest {
    bool is_ready;
};

struct ReadyStatusNotification {
    uint32_t user_id;
    bool is_ready;
};

struct GameStartNotification {
    uint8_t player_count;
    PlayerInfo players[MAX_PLAYERS_PER_ROOM];
};

struct QuestionNotification {
    uint32_t question_id;
    uint8_t time_limit_sec;
    char content[MAX_QUESTION_CONTENT_LEN];
    char options[4][MAX_OPTION_CONTENT_LEN];
};

struct SubmitAnswerRequest {
    uint32_t question_id;
    uint8_t selected_option;
    uint32_t response_time_ms;
};

struct PlayerRoundResult {
    uint32_t user_id;
    int32_t score_change;
    uint32_t total_score;
    uint8_t correct_option;
    uint32_t points_for_this_question; 
    bool was_eliminated; 
    bool answered_question;
};

struct RoundResultNotification {
    uint8_t correct_option;
    uint8_t result_count;
    PlayerRoundResult results[MAX_PLAYERS_PER_ROOM];
};

struct PlayerEliminatedNotification {
    uint32_t user_id;
    uint32_t session_id;
};

struct PlayerFinalResult {
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
    uint32_t final_rank;
    uint32_t final_score;
};

struct GameOverNotification {
    uint8_t result_count;
    PlayerFinalResult results[MAX_PLAYERS_PER_ROOM];
};

struct UserStatsResponse {
    uint32_t total_matches;
    uint32_t wins;
    uint32_t total_correct_answers;
    uint32_t total_incorrect_answers;
    double average_score;

    // We will need changes here based on how we calculates our score, implement later
};

struct ErrorResponse {
    StatusCode code;
    char message[MAX_ERROR_MSG_LEN];
};

#pragma pack(pop)
```

##  server/src/database_init.cpp 

```cpp
#include <iostream>

```

---

##  server/src/main.cpp 

```cpp
#include "network/Server.h"
#include "db/DatabaseManager.h"
#include <iostream>

int main(int argc, char** argv) {
    DatabaseManager::getInstance();
    
    int port = 10002;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    Server server(port);
    server.run();

    return 0;
}
```

---

##  server/src/db/UserRepository.cpp 

```cpp
#include <iostream>

```

---

##  server/src/db/QuestionRepository.cpp 

```cpp
#include <iostream>

```

---

##  server/src/db/DatabaseManager.h 

```cpp
#pragma once

#include <string>
#include <mutex>
#include <optional>
#include <memory>
#include <sqlite_modern_cpp.h>
#include "protocol.h"

struct UserData {
    uint32_t id;
    std::string email;
    std::string display_name;
};

class DatabaseManager {
public:
    static DatabaseManager& getInstance();
    
    StatusCode registerUser(const std::string& email, const std::string& display_name, const std::string& password);
    std::optional<UserData> loginUser(const std::string& email, const std::string& password);

private:
    DatabaseManager();
    ~DatabaseManager() = default;
    
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    std::unique_ptr<sqlite::database> db;
    std::mutex dbMutex;
};
```

---

##  server/src/db/QuestionRepository.h 

```cpp
#include <iostream>

```

---

##  server/src/db/UserRepository.h 

```cpp
#include <iostream>

```

---

##  server/src/db/DatabaseManager.cpp 

```cpp
#include "DatabaseManager.h"
#include <iostream>
#include <functional>
#include "bcrypt.h"

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    try {
        db = std::make_unique<sqlite::database>("game_db.db");
        *db << "PRAGMA journal_mode = WAL;";
        *db << "PRAGMA synchronous = NORMAL;";
    } catch (const std::exception& e) {
        std::cerr << "Database Init Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

StatusCode DatabaseManager::registerUser(const std::string& email, const std::string& displayName, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    try {
        int count = 0;
        *db << "SELECT COUNT(*) FROM users WHERE email = ?" << email >> count;
        if (count > 0) return StatusCode::EMAIL_EXISTS;

        char salt[BCRYPT_HASHSIZE];
        char hash[BCRYPT_HASHSIZE];
        bcrypt_gensalt(12, salt);
        bcrypt_hashpw(password.c_str(), salt, hash);

        *db << "INSERT INTO users (email, display_name, hashed_password) VALUES (?, ?, ?)"
            << email << displayName << hash;

        return StatusCode::SUCCESS;
    } catch (const std::exception&) {
        return StatusCode::FAILURE_GENERIC;
    }
}

std::optional<UserData> DatabaseManager::loginUser(const std::string& email, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    try {
        std::string dbHash;
        uint32_t id = 0;
        std::string dbInfoName;
        bool found = false;

        *db << "SELECT id, display_name, hashed_password FROM users WHERE email = ?" 
            << email 
            >> [&](uint32_t u_id, std::string d_name, std::string h_pass) {
                id = u_id;
                dbInfoName = d_name;
                dbHash = h_pass;
                found = true;
            };

        if (found) {
            int password_match = bcrypt_checkpw(password.c_str(), dbHash.c_str());
            if (password_match == 0){
                return UserData{id, email, dbInfoName};
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Login Error: " << e.what() << std::endl;
    }
    return std::nullopt;
}
```

---

##  server/src/utils/PasswordUtils.h 

```cpp
#include <iostream>

```

---

##  server/src/utils/Logger.h 

```cpp
#include <iostream>

```

---

##  server/src/game/RoomManager.cpp 

```cpp
#include "RoomManager.h"
#include <bits/stdc++.h>

RoomManager::RoomManager() : nextRoomId(1) {}

Room* RoomManager::createRoom(uint32_t hostId, const CreateRoomRequest& req) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    uint32_t id = nextRoomId++;
    auto room = std::make_unique<Room>(id, hostId, req.room_name, req.game_mode, req.num_questions);
    Room* roomPtr = room.get();
    
    activeRooms[id] = std::move(room);
    return roomPtr;
}

Room* RoomManager::getRoom(uint32_t roomId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = activeRooms.find(roomId);
    if (it != activeRooms.end()) {
        return it->second.get();
    }
    return nullptr;
}

void RoomManager::getAllRooms(ListRoomsResponse& response) {
    std::lock_guard<std::mutex> lock(managerMutex);
    response.room_count = 0;
    
    for (const auto& pair : activeRooms) {
        if (response.room_count >= MAX_ROOMS_PER_LIST) break;
        response.rooms[response.room_count++] = pair.second->getRoomInfo();
    }
}

void RoomManager::removeRoom(uint32_t roomId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    activeRooms.erase(roomId);
}

bool RoomManager::leaveRoom(uint32_t roomId, uint32_t userId) {
    bool isEmpty = false;
    {
        std::lock_guard<std::mutex> lock(managerMutex);
        auto it = activeRooms.find(roomId);
        if (it != activeRooms.end()) {
            it->second->removePlayer(userId);
            isEmpty = it->second->isEmpty();
        }
    }
    if (isEmpty) {
        removeRoom(roomId);
    }
    return isEmpty;
}
```

---

##  server/src/game/RoomManager.h 

```cpp
#pragma once

#include <unordered_map>
#include <mutex>
#include <memory>
#include <atomic>
#include "Room.h"

class RoomManager {
public:
    RoomManager();
    
    Room* createRoom(uint32_t hostId, const CreateRoomRequest& req);
    Room* getRoom(uint32_t roomId);
    void getAllRooms(ListRoomsResponse& response);
    void removeRoom(uint32_t roomId);
    
    bool leaveRoom(uint32_t roomId, uint32_t userId);

private:
    std::mutex managerMutex;
    std::unordered_map<uint32_t, std::unique_ptr<Room>> activeRooms;
    std::atomic<uint32_t> nextRoomId;
};
```

---

##  server/src/game/Room.cpp 

```cpp
#include <bits/stdc++.h>
#include "../network/ClientSession.h"
#include "Room.h"

Room::Room(uint32_t id, uint32_t hostId, std::string name, GameMode mode, uint8_t questions): roomId(id), hostUserId(hostId), roomName(std::move(name)), gameMode(mode), numQuestions(questions), maxPlayers(MAX_PLAYERS_PER_ROOM), state(RoomState::WAITING) {}

uint32_t Room::getId() const {return roomId;}
uint32_t Room::getHostId() const {return hostUserId;}

bool Room::isFull() {
    std::lock_guard<std::mutex> lock(roomMutex);
    return participants.size() >= maxPlayers;
}

bool Room::isEmpty(){
    std::lock_guard<std::mutex> lock(roomMutex);
    return participants.empty();
}

bool Room::addPlayer(ClientSession* session){
    std::lock_guard<std::mutex> lock(roomMutex);

    if (participants.size() >= maxPlayers) return false;
    if (state != RoomState::WAITING) return false;

    uint32_t uid = session->getUserId();
    if (participants.find(uid) != participants.end()) return true;

    participants[uid] = {session, (uid==hostUserId)};

    PlayerInfo pInfo;
    pInfo.user_id = uid;
    pInfo.is_ready = (uid == hostUserId);
    std::string dName = session->getDisplayName();
    std::strncpy(pInfo.display_name, dName.c_str(), MAX_DISPLAY_NAME_LEN - 1);

    broadcast(MessageType::S2C_PLAYER_JOINED_NOTIF, &pInfo, sizeof(pInfo), uid);
    return true;
}

void Room::removePlayer(uint32_t userId) {
    std::lock_guard<std::mutex> lock(roomMutex);
    
    auto it = participants.find(userId);
    if (it == participants.end()) return;

    participants.erase(it);

    if (participants.empty()) return;

    if (userId == hostUserId) {
        hostUserId = participants.begin()->first;
    }

    PlayerLeftNotification notif;
    notif.user_id = userId;
    notif.new_host_user_id = hostUserId;
    
    broadcast(MessageType::S2C_PLAYER_LEFT_NOTIF, &notif, sizeof(notif));
}

bool Room::setPlayerReady(uint32_t userId, bool ready) {
    std::lock_guard<std::mutex> lock(roomMutex);
    
    auto it = participants.find(userId);
    if (it == participants.end()) return false;

    it->second.isReady = ready;

    ReadyStatusNotification notif;
    notif.user_id = userId;
    notif.is_ready = ready;

    broadcast(MessageType::S2C_READY_STATUS_NOTIF, &notif, sizeof(notif));
    return true;
}

RoomInfo Room::getRoomInfo() const {
    std::lock_guard<std::mutex> lock(roomMutex);
    RoomInfo info;
    info.room_id = roomId;
    std::strncpy(info.room_name, roomName.c_str(), MAX_ROOM_NAME_LEN - 1);
    info.current_players = static_cast<uint8_t>(participants.size());
    info.max_players = maxPlayers;
    info.game_mode = gameMode;
    info.is_in_game = (state != RoomState::WAITING);
    return info;
}

void Room::getPlayerList(JoinRoomResponse& response) const {
    std::lock_guard<std::mutex> lock(roomMutex);
    response.player_count = 0;
    
    for (const auto& pair : participants) {
        if (response.player_count >= MAX_PLAYERS_PER_ROOM) break;
        
        PlayerInfo& p = response.players[response.player_count];
        p.user_id = pair.first;
        p.is_ready = pair.second.isReady;
        std::string dName = pair.second.session->getDisplayName();
        std::strncpy(p.display_name, dName.c_str(), MAX_DISPLAY_NAME_LEN - 1);
        
        response.player_count++;
    }
}

void Room::broadcast(MessageType type, const void* data, uint32_t len, uint32_t excludeUserId) {
    for (auto& pair : participants) {
        if (pair.first != excludeUserId) {
            pair.second.session->sendMsg(type, data, len);
        }
    }
}
```

---

##  server/src/game/Room.h 

```cpp
#pragma once

#include <bits/stdc++.h>
#include "protocol.h"

class ClientSession;

enum class RoomState {
    WAITING,
    STARTING,
    IN_GAME,
    FINISHED
};

class Room {
public:
    Room(uint32_t id, uint32_t hostId, std::string name, GameMode mode, uint8_t questions);
    ~Room() = default;

    uint32_t getId() const;
    uint32_t getHostId() const;
    bool isFull();
    bool isEmpty();
    
    bool addPlayer(ClientSession* session);
    void removePlayer(uint32_t userId);
    bool setPlayerReady(uint32_t userId, bool ready);
    
    RoomInfo getRoomInfo() const;
    void getPlayerList(JoinRoomResponse& response) const;

    void broadcast(MessageType type, const void* data, uint32_t len, uint32_t excludeUserId = 0);

private:
    mutable std::mutex roomMutex;

    uint32_t roomId;
    uint32_t hostUserId;
    std::string roomName;
    GameMode gameMode;
    uint8_t numQuestions;
    uint8_t maxPlayers;
    RoomState state;

    struct PlayerEntry {
        ClientSession* session;
        bool isReady;
    };
    std::map<uint32_t, PlayerEntry> participants;
};

```

---

##  server/src/network/ClientSession.cpp 

```cpp
#include "ClientSession.h"
#include "Server.h"
#include "../db/DatabaseManager.h"
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
        JoinRoomResponse rsp;
        rsp.code = StatusCode::FAILURE_GENERIC; 
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    Room* room = server->getRoomManager()->getRoom(req->room_id);
    if (!room) {
        JoinRoomResponse rsp;
        rsp.code = StatusCode::ROOM_NOT_FOUND;
        sendResponse(MessageType::S2C_JOIN_ROOM_RSP, &rsp, sizeof(rsp));
        return;
    }

    if (room->isFull()) {
        JoinRoomResponse rsp;
        rsp.code = StatusCode::ROOM_FULL;
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
        JoinRoomResponse rsp;
        rsp.code = StatusCode::GAME_IN_PROGRESS;
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
```

---

##  server/src/network/Server.h 

```cpp
#pragma once

#include <sys/epoll.h>
#include <map>
#include <memory>
#include "ClientSession.h"
#include "../game/RoomManager.h"

class Server {
public:
    Server(int port);
    ~Server();

    void run();
    RoomManager* getRoomManager();

private:
    void setNonBlocking(int fd);
    void handleAccept();

    int listenerFd;
    int epollFd;
    std::map<int, std::unique_ptr<ClientSession>> sessions;
    std::unique_ptr<RoomManager> roomManager;

    static const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];
};
```

---

##  server/src/network/ClientSession.h 

```cpp
#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include "protocol.h"

class Server;

struct SessionState {
    bool isAuthenticated = false;
    uint32_t userId = 0;
    std::string displayName;
    uint32_t currentRoomId = 0;
};

class ClientSession {
public:
    ClientSession(int fd, Server* server);
    ~ClientSession();

    void readData();
    bool wantWrite() const;
    void writeData();
    int getFd() const { return clientFd; }
    bool isMarkedForDeletion() const { return markedForDeletion; }

    uint32_t getUserId() const { return state.userId; }
    std::string getDisplayName() const { return state.displayName; }
    void sendMsg(MessageType type, const void* data, uint32_t len);
    void onDisconnect();
private:
    void processBuffer();
    void handleMessage(const MessageHeader& header, const std::vector<uint8_t>& body);
    
    void handleRegister(const RegisterRequest* req);
    void handleLogin(const LoginRequest* req);
    void handleCreateRoom(const CreateRoomRequest* req);
    void handleListRooms();
    void handleJoinRoom(const JoinRoomRequest* req);
    void handleLeaveRoom();
    void handleReadyStatus(const ReadyStatusRequest* req);

    void sendResponse(MessageType type, const void* data, uint32_t len);

    int clientFd;
    Server* server;
    bool markedForDeletion;
    
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> sendBuffer;

    SessionState state;
};
```

---

##  server/src/network/Server.cpp 

```cpp
#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstring>

Server::Server(int port) {
    roomManager = std::make_unique<RoomManager>();

    listenerFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerFd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listenerFd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    setNonBlocking(listenerFd);

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listenerFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listenerFd, SOMAXCONN) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    epollFd = epoll_create1(0);
    if (epollFd < 0) {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenerFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, listenerFd, &ev);

    std::cout << "Server listening on port " << port << std::endl;
}

Server::~Server() {
    close(listenerFd);
    close(epollFd);
}

RoomManager* Server::getRoomManager(){
    return roomManager.get();
}

void Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::run() {
    while (true) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == listenerFd) {
                handleAccept();
            } else {
                auto it = sessions.find(fd);
                if (it != sessions.end()) {
                    if (events[i].events & EPOLLIN) {
                        it->second->readData();
                    }
                    if (events[i].events & EPOLLOUT) {
                        it->second->writeData();
                    }
                    
                    if (it->second->isMarkedForDeletion()) {
                        it->second->onDisconnect();
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
                        sessions.erase(it);
                    } else {
                        struct epoll_event ev;
                        ev.events = EPOLLIN | EPOLLET;
                        if (it->second->wantWrite()) {
                            ev.events |= EPOLLOUT;
                        }
                        ev.data.fd = fd;
                        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
                    }
                }
            }
        }
    }
}

void Server::handleAccept() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(listenerFd, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientFd < 0) return;

    setNonBlocking(clientFd);
    
    sessions[clientFd] = std::make_unique<ClientSession>(clientFd, this);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev);
}
```

---

