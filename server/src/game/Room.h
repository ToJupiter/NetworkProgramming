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
