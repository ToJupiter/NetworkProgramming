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