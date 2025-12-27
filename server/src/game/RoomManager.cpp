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

bool RoomManager::leaveRoom(uint32_t roomId, uint32_t userId, bool isHost) {
    bool remove = false;
    {
        std::lock_guard<std::mutex> lock(managerMutex);
        auto it = activeRooms.find(roomId);
        if (it != activeRooms.end()) {
            Room* room = it->second.get();

            if (isHost) {
                // Host leaves: notify all clients and mark room for removal
                room->broadcastRoomClosed();
                remove = true;
            } else {
                // Regular player leaves: remove and notify remaining players
                room->removePlayer(userId);
                room->sendPlayerListUpdate();
                remove = room->isEmpty();
            }
        }
    }
    if (remove) {
        removeRoom(roomId);
    }
    return remove;
}

void RoomManager::updateAllRooms() {
    std::lock_guard<std::mutex> lock(managerMutex);

    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (auto it = activeRooms.begin(); it != activeRooms.end(); ) {
        it->second->update(now);
        ++it;
    } 
}