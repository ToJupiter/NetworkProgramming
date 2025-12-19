#include <bits/stdc++.h>
#include "../network/ClientSession.h"
#include "Room.h"

Room::Room(uint32_t id, uint32_t hostId, std::string name, GameMode mode, uint8_t questions): roomId(id), hostUserId(hostId), roomName(std::move(name)), gameMode(mode), numQuestions(questions), maxPlayers(MAX_PLAYERS_PER_ROOM), state(RoomState::WAITING), stateStartTimeMs(0), currentQuestionIndex(0) {}

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
    if (participants.count(uid)) return true;

    PlayerGameData pd;
    pd.session = session;
    pd.isReady = (uid == hostUserId);
    participants[uid] = pd;

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

    if (participants.empty()) {
        state = RoomState::FINISHED;    
        return;
    }

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

    if (participants.size() >= 2) {
        bool allReady = true;
        for (const auto& p : participants) {
            if (!p.second.isReady) { allReady = false; break; }
        }
        if (allReady) {
            startGame();
        }
    }
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

void Room::startGame() {
    state = RoomState::STARTING;
    stateStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    QuestionRepository repo;
    questions = repo.getRandomQuestions(numQuestions);

    GameStartNotification notif;
    notif.player_count = 0;
    for (const auto &p : participants) {
        PlayerInfo &pi = notif.players[notif.player_count++];
        pi.user_id = p.first;
        pi.is_ready = true;
        std::strncpy(pi.display_name, p.second.session->getDisplayName().c_str(), MAX_DISPLAY_NAME_LEN - 1);
    }

    broadcast(MessageType::S2C_GAME_START_NOTIF, &notif, sizeof(notif));
}

void Room::update(uint64_t nowMs) {
    std::lock_guard<std::mutex> lock(roomMutex);

    if (state == RoomState::STARTING) {
        if (nowMs >= stateStartTimeMs + START_DELAY_MS) {
            currentQuestionIndex = 0;
            nextRound();
        }
    } else if (state == RoomState::IN_GAME_QUESTION) {
        if (nowMs >= stateStartTimeMs + QUESTION_TIME_LIMIT_MS + 500) endRound();
        else if (allActivePlayersAnswered()) endRound();
    } else if (state == RoomState::IN_GAME_RESULT) {
        if (nowMs >= stateStartTimeMs + RESULT_DISPLAY_MS) {
            currentQuestionIndex++;
            if (currentQuestionIndex >= questions.size()) finishGame();
            else nextRound();
        }
    }
}

void Room::nextRound() {
    state = RoomState::IN_GAME_QUESTION;
    stateStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (auto &p : participants) {
        p.second.hasAnswered = false;
        p.second.selectedOption = 0;
        p.second.lastScoreChange = 0;
    }

    if (currentQuestionIndex >= questions.size()){
        finishGame();
        return;
    }

    const auto &q = questions[currentQuestionIndex];
    QuestionNotification notif;
    notif.question_id = q.id;
    notif.time_limit_sec = static_cast<uint8_t>(QUESTION_TIME_LIMIT_MS / 1000);
    std::strncpy(notif.content, q.content.c_str(), MAX_QUESTION_CONTENT_LEN - 1);
    for (int i = 0; i < 4; i++) {
        std::strncpy(notif.options[i], q.options[i].c_str(), MAX_OPTION_CONTENT_LEN - 1);
    }

    broadcast(MessageType::S2C_QUESTION_NOTIF, &notif, sizeof(notif));
}

