#pragma once

#include <bits/stdc++.h>
#include "protocol.h"
#include "../db/QuestionRepository.h"

class ClientSession;

enum class RoomState {
    WAITING,
    STARTING,
    IN_GAME_QUESTION,
    IN_GAME_RESULT,
    FINISHED
};

struct PlayerGameData {
    ClientSession* session;
    bool isReady = false;
    uint32_t score = 0;
    bool isEliminated = false;
    
    bool hasAnswered = false;
    uint32_t lastResponseTimeMs = 0;
    uint8_t selectedOption = 0;
    int32_t lastScoreChange = 0;
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

    // Main gameplay (submit answer, auto-update state on server)
    void handleSubmitAnswer(uint32_t userId, const SubmitAnswerRequest& req);
    void update(uint64_t nowMs);
    
    RoomInfo getRoomInfo() const;
    void getPlayerList(JoinRoomResponse& response) const;


private:
    void broadcast(MessageType type, const void* data, uint32_t len, uint32_t excludeUserId = 0);

    void startGame();
    void nextRound();
    void endRound();
    void finishGame();

    void calculateScores();
    bool allActivePlayersAnswered();

    mutable std::mutex roomMutex;

    uint32_t roomId;
    uint32_t hostUserId;
    std::string roomName;
    GameMode gameMode;
    uint8_t numQuestions;
    uint8_t maxPlayers;
    RoomState state;
    uint64_t stateStartTimeMs;

    std::map<uint32_t, PlayerGameData> participants;
    std::vector<Question> questions;
    uint8_t currentQuestionIndex;

    const uint64_t START_DELAY_MS = 5000;
    const uint64_t QUESTION_TIME_LIMIT_MS = 15000;
    const uint64_t RESULT_DISPLAY_MS = 3000;

    // struct PlayerEntry {
    //     ClientSession* session;
    //     bool isReady;
    // };
    // std::map<uint32_t, PlayerEntry> participants;
};
