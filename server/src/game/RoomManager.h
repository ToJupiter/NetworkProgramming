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
    void updateAllRooms();

private:
    std::mutex managerMutex;
    std::unordered_map<uint32_t, std::unique_ptr<Room>> activeRooms;
    std::atomic<uint32_t> nextRoomId;
};