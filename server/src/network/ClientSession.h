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
    bool isRoomHost = false;
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
    void handleStartGame();
    void handleSubmitAnswer(const SubmitAnswerRequest* req);
    void handleReturnToRoom();
    void handleGetStats();
    void handlePauseGame();
    void handleResumeGame();

    void sendResponse(MessageType type, const void* data, uint32_t len);

    int clientFd;
    Server* server;
    bool markedForDeletion;
    
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> sendBuffer;

    SessionState state;
};