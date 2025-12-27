#include <bits/stdc++.h>
#include "../network/ClientSession.h"
#include "Room.h"
#include "../db/DatabaseManager.h"
#include "../db/UserRepository.h"

Room::Room(uint32_t id, uint32_t hostId, std::string name, GameMode mode, uint8_t questions): roomId(id), hostUserId(hostId), roomName(std::move(name)), gameMode(mode), numQuestions(questions), maxPlayers(MAX_PLAYERS_PER_ROOM), state(RoomState::WAITING), stateStartTimeMs(0), currentQuestionIndex(0), previousState(RoomState::WAITING), pauseStartTimeMs(0), totalPauseDurationMs(0) {}

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
        if (state == RoomState::WAITING){
            hostUserId = participants.begin()->first;
            PlayerLeftNotification notif;
            notif.user_id = userId;
            notif.new_host_user_id = hostUserId;
            broadcast(MessageType::S2C_PLAYER_LEFT_NOTIF, &notif, sizeof(notif));
        } else {
            terminateGame(TerminationReason::HOST_LEFT);
            return;
        }
    } else {
        PlayerLeftNotification notif;
        notif.user_id = userId;
        notif.new_host_user_id = hostUserId;
        broadcast(MessageType::S2C_PLAYER_LEFT_NOTIF, &notif, sizeof(notif));
    }

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

    // Auto-start disabled; wait for explicit C2S_START_GAME_REQ from host
    return true;
}

void Room::handleStartGame(uint32_t userId) {
    std::lock_guard<std::mutex> lock(roomMutex);
    
    // Only host can start the game
    if (userId != hostUserId) {
        std::cout << "[Room " << roomId << "] Non-host user " << userId 
                  << " attempted to start game (denied)" << std::endl;
        return;
    }
    
    // Check if all players are ready
    if (participants.size() < 2) {
        std::cout << "[Room " << roomId << "] Cannot start: need at least 2 players" << std::endl;
        return;
    }
    
    bool allReady = true;
    for (const auto& p : participants) {
        if (!p.second.isReady) {
            allReady = false;
            break;
        }
    }
    
    if (!allReady) {
        std::cout << "[Room " << roomId << "] Cannot start: not all players ready" << std::endl;
        return;
    }
    
    std::cout << "[Room " << roomId << "] Host starting game with " 
              << participants.size() << " players" << std::endl;
    startGame();
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
            // Immediately flush to avoid epoll edge-triggered issues
            pair.second.session->writeData();
        }
    }
}

void Room::startGame() {
    state = RoomState::STARTING;
    stateStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    totalPauseDurationMs = 0;
    
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

    if (state == RoomState::PAUSED || state == RoomState::WAITING || state == RoomState::FINISHED) return;

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

void Room::handleSubmitAnswer(uint32_t userId, const SubmitAnswerRequest& req) {
    std::lock_guard<std::mutex> lock(roomMutex);

    if (state != RoomState::IN_GAME_QUESTION) return;
    auto it = participants.find(userId);
    if (it == participants.end()) return;
    if (it->second.isEliminated || it->second.hasAnswered) return;

    uint64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    uint64_t measuredLatency = static_cast<uint64_t>(nowMs - stateStartTimeMs);
    uint32_t clientReportedTime = req.response_time_ms;
    uint64_t maxAllowed = static_cast<uint32_t>(QUESTION_TIME_LIMIT_MS + 500);

    it->second.hasAnswered = true;
    it->second.selectedOption = req.selected_option;
    
    if (clientReportedTime > maxAllowed || clientReportedTime < (measuredLatency > 200 ? measuredLatency - 200 : 0))
        it->second.lastResponseTimeMs = static_cast<uint32_t>(measuredLatency);
    else
        it->second.lastResponseTimeMs = clientReportedTime;

    pendingLogs.push_back({
        userId,
        questions[currentQuestionIndex].id,
        req.selected_option,
        (req.selected_option == questions[currentQuestionIndex].correct_option),
        it->second.lastResponseTimeMs
    });
}

void Room::endRound() {
    calculateScores();
    state = RoomState::IN_GAME_RESULT;
    stateStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    RoundResultNotification notif;
    notif.correct_option = questions[currentQuestionIndex].correct_option;
    notif.result_count = 0;

    for (const auto &p: participants) {
        PlayerRoundResult& res = notif.results[notif.result_count++];
        res.user_id = p.first;
        res.score_change = p.second.lastScoreChange;
        res.total_score = p.second.score;
        res.correct_option = questions[currentQuestionIndex].correct_option;
        res.points_for_this_question = (p.second.lastScoreChange > 0) ? p.second.lastScoreChange : 0;
        res.was_eliminated = p.second.isEliminated;
        res.answered_question = p.second.hasAnswered;
    }

    broadcast(MessageType::S2C_ROUND_RESULT_NOTIF, &notif, sizeof(notif));

    if (gameMode == GameMode::ELIMINATION) {
        for (const auto &p : participants) {
            if (p.second.isEliminated && p.second.lastScoreChange == -1) {
                PlayerEliminatedNotification elimNotif;
                elimNotif.user_id = p.first;
                elimNotif.session_id = 0;
                broadcast(MessageType::S2C_PLAYER_ELIMINATED_NOTIF, &elimNotif, sizeof(elimNotif));
            }
        }
    }
}

void Room::calculateScores() {
    uint8_t correctOpt = questions[currentQuestionIndex].correct_option;

    for (auto& pair: participants) {
        auto& p = pair.second;
        if (p.isEliminated) continue;

        bool correct = p.hasAnswered && (p.selectedOption == correctOpt);

        if (gameMode == GameMode::SCORING) {
            if (correct) {
                double ratio = (double)p.lastResponseTimeMs / QUESTION_TIME_LIMIT_MS;
                if (ratio > 1.0) ratio = 1.0;
                int points = 1000 - (int)(500 * ratio);
                p.score += points;
                p.lastScoreChange = points;
            }
            else p.lastScoreChange = 0;
        }

        else if (gameMode == GameMode::ELIMINATION) {
            if (!correct) {
                p.isEliminated = true;
                p.lastScoreChange = -1;
            } else {
                p.score += 100;
                p.lastScoreChange = 100;
            }
        }
    }
}

void Room::finishGame() {
    state = RoomState::FINISHED;

    std::vector<PlayerGameData*> sortedPlayers;
    for (auto& p: participants) sortedPlayers.push_back(&p.second);

    std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](PlayerGameData* a, PlayerGameData* b) {
        if (a->isEliminated != b->isEliminated) return !a->isEliminated;
        return a->score > b->score;
    });

    GameOverNotification notif;
    notif.result_count = 0;

    std::vector<UserRepository::RankUpdateInfo> eloUpdates;

    for (size_t i = 0; i < sortedPlayers.size(); ++i) {
        PlayerFinalResult& res = notif.results[notif.result_count++];
        res.user_id = sortedPlayers[i]->session->getUserId();
        std::strncpy(res.display_name, sortedPlayers[i]->session->getDisplayName().c_str(), MAX_DISPLAY_NAME_LEN - 1);
        res.final_rank = static_cast<uint32_t>(i + 1);
        res.final_score = sortedPlayers[i]->score;

        eloUpdates.push_back({res.user_id, res.final_rank, res.final_score});
    }

    broadcast(MessageType::S2C_GAME_OVER_NOTIF, &notif, sizeof(notif));

    persistResults(sortedPlayers, TerminationReason::UNKNOWN);
    UserRepository::updateUserRanks(eloUpdates);
}

void Room::persistResults(const std::vector<PlayerGameData*>& sortedPlayers, TerminationReason reason) {
    try {
        auto& db = DatabaseManager::getInstance().getDb();
        
        db << "BEGIN TRANSACTION;";

        std::string modeStr = (gameMode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
        db << "INSERT INTO game_sessions (game_mode, ended_at, total_pause_duration_ms) VALUES (?, CURRENT_TIMESTAMP, ?);" 
           << modeStr << static_cast<long long>(totalPauseDurationMs);
        
        uint32_t sessionId = 0;
        db << "SELECT last_insert_rowid();" >> sessionId;

        for (size_t i = 0; i < sortedPlayers.size(); ++i) {
            db << "INSERT INTO session_participants (user_id, session_id, score, rank) VALUES (?, ?, ?, ?);"
               << sortedPlayers[i]->session->getUserId()
               << sessionId
               << sortedPlayers[i]->score
               << static_cast<uint32_t>(i + 1);
        }

        for (const auto& log : pendingLogs) {
            db << "INSERT INTO game_log (session_id, user_id, question_id, selected_option, is_correct, response_time_ms) VALUES (?, ?, ?, ?, ?, ?);"
               << sessionId
               << log.user_id
               << log.question_id
               << static_cast<int>(log.selected_option)
               << (log.is_correct ? 1 : 0)
               << log.response_time_ms;
        }

        db << "COMMIT;";
    } catch (const std::exception& e) {
        DatabaseManager::getInstance().getDb() << "ROLLBACK;";
        std::cerr << "Persistence Error: " << e.what() << std::endl;
    }
}

bool Room::allActivePlayersAnswered() {
    for (const auto &p : participants) {
        if (!p.second.isEliminated && !p.second.hasAnswered) return false;
    }
    return true;
}

void Room::handlePauseGame(uint32_t userId) {
    std::lock_guard<std::mutex> lock(roomMutex);
    if (userId != hostUserId) return;
    if (state != RoomState::IN_GAME_QUESTION && state != RoomState::IN_GAME_RESULT) return;

    previousState = state;
    state = RoomState::PAUSED;
    pauseStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    GamePausedNotification notif;
    broadcast(MessageType::S2C_GAME_PAUSED_NOTIF, &notif, sizeof(notif));
}

void Room::handleResumeGame(uint32_t userId) {
    std::lock_guard<std::mutex> lock(roomMutex);
    if (userId != hostUserId || state != RoomState::PAUSED) return;

    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    uint64_t duration = now - pauseStartTimeMs;
    totalPauseDurationMs += duration;

    stateStartTimeMs += duration;

    state = previousState;

    GameResumedNotification notif;
    broadcast(MessageType::S2C_GAME_RESUMED_NOTIF, &notif, sizeof(notif));
}

void Room::terminateGame(TerminationReason reason) {
    state = RoomState::FINISHED;

    GameTerminatedNotification notif;
    notif.reason = reason;
    broadcast(MessageType::S2C_GAME_TERMINATED_NOTIF, &notif, sizeof(notif));

    std::vector<PlayerGameData*> sortedPlayers;
    for (auto &p: participants) sortedPlayers.push_back(&p.second);
    std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](PlayerGameData* a , PlayerGameData* b){
        if (a->isEliminated != b->isEliminated) return !a->isEliminated;
        return a->score > b->score;
    });

    persistResults(sortedPlayers, reason);
}