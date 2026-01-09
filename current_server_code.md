# Source Code Collection
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
    C2S_START_GAME_REQ,

    S2C_GAME_START_NOTIF,
    S2C_QUESTION_NOTIF,
    C2S_SUBMIT_ANSWER_REQ,
    S2C_ROUND_RESULT_NOTIF,
    S2C_PLAYER_ELIMINATED_NOTIF,
    S2C_GAME_OVER_NOTIF,
    C2S_LEAVE_MATCH_REQ,
    C2S_RETURN_TO_ROOM_REQ,
    S2C_RETURN_TO_ROOM_RSP,

    C2S_GET_STATS_REQ,
    S2C_GET_STATS_RSP,

    C2S_PAUSE_GAME_REQ,
    S2C_GAME_PAUSED_NOTIF,
    C2S_RESUME_GAME_REQ,
    S2C_GAME_RESUMED_NOTIF,
    S2C_GAME_TERMINATED_NOTIF,

    C2S_GET_REPLAY_REQ,
    S2C_GET_REPLAY_RSP,

    C2S_GET_GAME_HISTORY_REQ,
    S2C_GET_GAME_HISTORY_RSP,

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


enum class TerminationReason : uint8_t {
    HOST_LEFT,
    NOT_ENOUGH_PLAYERS,
    SERVER_SHUTDOWN,
    UNKNOWN
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
    bool is_winner;
};

enum class GameEndReason : uint8_t {
    SINGLE_WINNER,
    DRAW,
    NO_WINNER_WIPEOUT
};

struct GameOverNotification {
    uint8_t result_count;
    PlayerFinalResult results[MAX_PLAYERS_PER_ROOM];
    GameEndReason end_reason;
    uint8_t winner_count;
};

struct UserModeStats {
    uint32_t total_matches;
    uint32_t wins;
    uint32_t total_correct_answers;
    uint32_t total_incorrect_answers;
    double average_score;
    uint32_t high_score;
};

struct UserStatsResponse {
    UserModeStats elimination;
    UserModeStats scoring;
    uint32_t ranked_points;
    uint32_t total_ranked_players;
    uint32_t player_rank;
};

struct GetReplayRequest {
    uint32_t session_id;
};

struct ReplayEvent {
    uint32_t timestamp_ms;
    uint32_t question_id;
    uint32_t user_id;
    uint8_t selected_option;
    bool is_correct;
    uint32_t response_time_ms;
    char question_content[MAX_QUESTION_CONTENT_LEN];
    char options[4][MAX_OPTION_CONTENT_LEN];
    uint8_t correct_option;
    uint8_t difficulty;
};

struct ReplayDataResponse {
    StatusCode status;
    uint32_t session_id;
    GameMode game_mode;
    uint32_t event_count;
};

struct GameHistoryEntry {
    uint32_t session_id;
    char game_mode[16];
    uint32_t player_score;
    uint32_t player_rank;
    uint32_t correct_answers;
    uint32_t total_questions;
    uint32_t avg_response_time_ms;
    uint64_t timestamp_sec;
};

struct GameHistoryResponse {
    StatusCode status;
    uint32_t entry_count;
    GameHistoryEntry entries[100];
};

struct PauseGameRequest {
    // Empty body
};

struct GamePausedNotification {
    // Empty body
};

struct ResumeGameRequest {
    // Empty body
};

struct GameResumedNotification {
    // Empty body
};

struct GameTerminatedNotification {
    TerminationReason reason;
};

struct ErrorResponse {
    StatusCode code;
    char message[MAX_ERROR_MSG_LEN];
};

#pragma pack(pop)
```


## database_init.sql
``` sql
-- schema.sql

-- Table to store user account information
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    email TEXT UNIQUE NOT NULL,
    display_name TEXT NOT NULL,
    hashed_password TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    ranked_points INTEGER DEFAULT 1000
);

-- Table to store all quiz questions
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content TEXT NOT NULL,
    option1 TEXT NOT NULL,
    option2 TEXT NOT NULL,
    option3 TEXT NOT NULL,
    option4 TEXT NOT NULL,
    correct_option INTEGER NOT NULL, -- 1, 2, 3, or 4
    difficulty INTEGER DEFAULT 1 -- e.g., 1 for easy, 2 for medium, etc.
);

-- Table to log each game session/match that occurs
CREATE TABLE IF NOT EXISTS game_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    game_mode TEXT NOT NULL, -- e.g., 'Elimination', 'Scoring'
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    ended_at DATETIME
);

-- Table to link users to game sessions and store their results
-- This table is essential for tracking history and statistics
CREATE TABLE IF NOT EXISTS session_participants (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_id INTEGER NOT NULL,
    score INTEGER DEFAULT 0,
    rank INTEGER, -- Final rank in the game (e.g., 1 for winner)
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(session_id) REFERENCES game_sessions(id)
);

-- Table to log every single answer from every player (for replay functionality)
CREATE TABLE IF NOT EXISTS game_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    question_id INTEGER NOT NULL,
    selected_option INTEGER NOT NULL,
    is_correct BOOLEAN NOT NULL,
    response_time_ms INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(session_id) REFERENCES game_sessions(id),
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(question_id) REFERENCES questions(id)
);
```

## CMakeLists.txt
``` cmake
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
    src/db/QuestionRepository.cpp
    src/db/UserRepository.cpp
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


##  server/main.cpp 

```cpp
#include "network/Server.h"
#include "db/DatabaseManager.h"
#include <sqlite_modern_cpp.h>
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

##  server/database_init.cpp 

```cpp
#include "db/DatabaseManager.h"
#include <iostream>

void initializeDatabase() {
    try {
        auto& db = DatabaseManager::getInstance().getDb();
        
        db << "CREATE TABLE IF NOT EXISTS users ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "email TEXT UNIQUE NOT NULL, "
              "display_name TEXT NOT NULL, "
              "hashed_password TEXT NOT NULL, "
              "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "ranked_points INTEGER DEFAULT 1000"
              ");";

        // Questions Table
        db << "CREATE TABLE IF NOT EXISTS questions ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "content TEXT NOT NULL, "
              "option1 TEXT NOT NULL, "
              "option2 TEXT NOT NULL, "
              "option3 TEXT NOT NULL, "
              "option4 TEXT NOT NULL, "
              "correct_option INTEGER NOT NULL, "
              "difficulty INTEGER DEFAULT 1"
              ");";

        // Game Sessions Table
        // FIXED: Added total_pause_duration_ms to match Room.cpp persistence logic
        db << "CREATE TABLE IF NOT EXISTS game_sessions ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "game_mode TEXT NOT NULL, "
              "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "ended_at DATETIME"
              ");";

        // Session Participants Table
        db << "CREATE TABLE IF NOT EXISTS session_participants ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "user_id INTEGER NOT NULL, "
              "session_id INTEGER NOT NULL, "
              "score INTEGER DEFAULT 0, "
              "rank INTEGER, "
              "FOREIGN KEY(user_id) REFERENCES users(id), "
              "FOREIGN KEY(session_id) REFERENCES game_sessions(id)"
              ");";

        // Game Log Table
        db << "CREATE TABLE IF NOT EXISTS game_log ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "session_id INTEGER NOT NULL, "
              "user_id INTEGER NOT NULL, "
              "question_id INTEGER NOT NULL, "
              "selected_option INTEGER NOT NULL, "
              "is_correct BOOLEAN NOT NULL, "
              "response_time_ms INTEGER NOT NULL, "
              "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "FOREIGN KEY(session_id) REFERENCES game_sessions(id), "
              "FOREIGN KEY(user_id) REFERENCES users(id), "
              "FOREIGN KEY(question_id) REFERENCES questions(id)"
              ");";
              
        std::cout << "Database initialized successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Database Initialization Failed: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
```

---

##  server/db/DatabaseManager.cpp 

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

##  server/db/QuestionRepository.cpp 

```cpp
#include "QuestionRepository.h"
#include "DatabaseManager.h"
#include <iostream>

std::vector<Question> QuestionRepository::getRandomQuestions(int count) {
    std::vector<Question> questions;
    try {
        auto& db = DatabaseManager::getInstance().getDb();

        db << "SELECT id, content, option1, option2, option3, option4, correct_option, difficulty "
              "FROM questions ORDER BY RANDOM() LIMIT ?"
           << count
           >> [&](uint32_t id, std::string content, std::string o1, std::string o2, std::string o3, std::string o4, int correct, int diff) {
               Question q;
               q.id = id;
               q.content = content;
               q.options[0] = o1;
               q.options[1] = o2;
               q.options[2] = o3;
               q.options[3] = o4;
               q.correct_option = static_cast<uint8_t>(correct);
               q.difficulty = static_cast<uint8_t>(diff);
               questions.push_back(q);
           };

    } catch (const std::exception &e) {
        std::cerr << "DB Error fetching questions: " << e.what() << std::endl;
    }

    return questions;
}
```

---

##  server/db/UserRepository.h 

```cpp
#pragma once

#include <bits/stdc++.h>
#include "protocol.h"

class UserRepository {
public:
    static UserStatsResponse getUserStats(uint32_t userId);

    struct RankUpdateInfo {
        uint32_t user_id;
        uint32_t rank;
        uint32_t score;
    };

    static void updateUserRanks(const std::vector<RankUpdateInfo>& matchResults);

private:
    static uint32_t calculateNewElo(uint32_t currentElo, uint32_t opponentElo, double actualScore);
};
```

---

##  server/db/QuestionRepository.h 

```cpp
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "protocol.h"

struct Question {
    uint32_t id;
    std::string content;
    std::string options[4];
    uint8_t correct_option;
    uint8_t difficulty;
};

class QuestionRepository {
public:
    std::vector<Question> getRandomQuestions(int count);
};
```

---

##  server/db/DatabaseManager.h 

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
    sqlite::database& getDb() { return *db; } 
    
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

##  server/db/UserRepository.cpp 

```cpp
#include "UserRepository.h"
#include "DatabaseManager.h"
#include <bits/stdc++.h>
#include <algorithm>

UserStatsResponse UserRepository::getUserStats(uint32_t userId) {
    UserStatsResponse stats{};
    stats.ranked_points = 1000;
    stats.total_ranked_players = 0;
    stats.player_rank = 0;

    auto fillModeStats = [&](const std::string& mode, UserModeStats& out) {
        out.total_matches = 0;
        out.wins = 0;
        out.total_correct_answers = 0;
        out.total_incorrect_answers = 0;
        out.average_score = 0.0;
        out.high_score = 0;

        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT COUNT(*), IFNULL(SUM(CASE WHEN rank = 1 THEN 1 ELSE 0 END), 0) "
              "FROM session_participants sp "
              "JOIN game_sessions gs ON sp.session_id = gs.id "
              "WHERE sp.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](int total, int wins) {
                out.total_matches = total;
                out.wins = wins;
           };

        db << "SELECT IFNULL(AVG(sp.score), 0), IFNULL(MAX(sp.score), 0) "
              "FROM session_participants sp "
              "JOIN game_sessions gs ON sp.session_id = gs.id "
              "WHERE sp.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](double avg, int maxScore) {
                out.average_score = avg;
                out.high_score = static_cast<uint32_t>(std::max(0, maxScore));
           };

        db << "SELECT IFNULL(SUM(CASE WHEN gl.is_correct = 1 THEN 1 ELSE 0 END), 0), "
              "       IFNULL(SUM(CASE WHEN gl.is_correct = 0 THEN 1 ELSE 0 END), 0) "
              "FROM game_log gl "
              "JOIN game_sessions gs ON gl.session_id = gs.id "
              "WHERE gl.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](int correct, int incorrect) {
                out.total_correct_answers = correct;
                out.total_incorrect_answers = incorrect;
           };
    };

    try {
        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT ranked_points FROM users WHERE id = ?"
           << userId
           >> stats.ranked_points;

        fillModeStats("Elimination", stats.elimination);
        fillModeStats("Scoring", stats.scoring);

        db << "SELECT COUNT(*) FROM users WHERE ranked_points > 0"
           >> stats.total_ranked_players;

        db << "SELECT COUNT(*) FROM users WHERE ranked_points > ?"
           << stats.ranked_points
           >> stats.player_rank;
        
        stats.player_rank += 1;
    } catch (const std::exception &e) {
        std::cerr << "DB Error getting stats: " << e.what() << std::endl;
    }

    return stats;
}

void UserRepository::updateUserRanks(const std::vector<RankUpdateInfo>& results) {
    if (results.size() < 2) return;

    try {
        auto &db = DatabaseManager::getInstance().getDb();
        db << "BEGIN TRANSACTION;";

        uint64_t totalElo = 0;
        std::vector<uint32_t> currentElos;

        for (const auto &p : results) {
            uint32_t elo = 1000;
            bool found = false;

            db << "SELECT ranked_points FROM users WHERE id = ? " << p.user_id
               >> [&](uint32_t points) {
                   elo = points;
                   found = true;
               };
               
            if (!found) {
                std::cerr << "User ID " << p.user_id << " not found. Aborting rank update." << std::endl;
                db << "ROLLBACK; ";
                return;
            }

            currentElos.push_back(elo);
            totalElo += elo;
        }

        double avgElo = static_cast<double>(totalElo) / results.size();

        for (size_t i = 0; i < results.size(); i++) {
            uint32_t userId = results[i].user_id;
            uint32_t oldElo = currentElos[i];
            uint32_t rank = results[i].rank;

            double actualScore = 1.0 - (double)(rank - 1) / (results.size() - 1);
            if (results.size() == 1) actualScore = 1.0;
            
            double expectedScore = 1.0 / (1.0 + pow(10.0, (avgElo - oldElo) / 400.0));
            int k = 32;

            int change = static_cast<int>(k * (actualScore - expectedScore));
            int newElo = static_cast<int>(oldElo) + change;
            if (newElo < 0) newElo = 0;


            db << "UPDATE users SET ranked_points = ? WHERE id = ?" 
               << newElo << userId;
        }
        
        db << "COMMIT;";
        
    } catch (const std::exception &e) {
        std::cerr << "DB Error updating ranks: " << e.what() << std::endl;
        DatabaseManager::getInstance().getDb() << "ROLLBACK;";
    }
}
```

---

##  server/game/Room.h 

```cpp
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
    PAUSED,
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
    void handleStartGame(uint32_t userId);
    void handleReturnToRoom(uint32_t userId);

    // Main gameplay (submit answer, auto-update state on server)
    void handleSubmitAnswer(uint32_t userId, const SubmitAnswerRequest& req);
    void handlePauseGame(uint32_t userId);
    void handleResumeGame(uint32_t userId);

    void update(uint64_t nowMs);
    
    RoomInfo getRoomInfo() const;
    void getPlayerList(JoinRoomResponse& response) const;


private:
    void broadcast(MessageType type, const void* data, uint32_t len, uint32_t excludeUserId = 0);

    void startGame();
    void nextRound();
    void endRound();
    void finishGame();
    void terminateGame(TerminationReason reason);

    void calculateScores();
    bool allActivePlayersAnswered();
    void persistResults(const std::vector<PlayerGameData*>& sortedPlayers, TerminationReason reason);

    mutable std::mutex roomMutex;

    uint32_t roomId;
    uint32_t hostUserId;
    std::string roomName;
    GameMode gameMode;
    uint8_t numQuestions;
    uint8_t maxPlayers;
    RoomState state;
    RoomState previousState;
    uint64_t stateStartTimeMs;
    uint64_t sessionStartTimeMs;

    uint64_t pauseStartTimeMs;
    uint64_t totalPauseDurationMs;

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

    struct LogEntry {
        uint32_t user_id;
        uint32_t question_id;
        uint8_t selected_option;
        bool is_correct;
        uint32_t response_time_ms;
    };
    std::vector<LogEntry> pendingLogs;
    uint32_t dbSessionId = 0;

};

```

---

##  server/game/RoomManager.h 

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
    void updateAllRooms();

private:
    std::mutex managerMutex;
    std::unordered_map<uint32_t, std::unique_ptr<Room>> activeRooms;
    std::atomic<uint32_t> nextRoomId;
};
```

---

##  server/game/RoomManager.cpp 

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

void RoomManager::updateAllRooms() {
    std::lock_guard<std::mutex> lock(managerMutex);

    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (auto it = activeRooms.begin(); it != activeRooms.end(); ) {
        it->second->update(now);
        ++it;
    } 
}
```

---

##  server/game/Room.cpp 

```cpp
#include <bits/stdc++.h>
#include "../network/ClientSession.h"
#include "Room.h"
#include "../db/DatabaseManager.h"
#include "../db/UserRepository.h"

Room::Room(uint32_t id, uint32_t hostId, std::string name, GameMode mode, uint8_t questions): roomId(id), hostUserId(hostId), roomName(std::move(name)), gameMode(mode), numQuestions(questions), maxPlayers(MAX_PLAYERS_PER_ROOM), state(RoomState::WAITING), stateStartTimeMs(0), currentQuestionIndex(0), previousState(RoomState::WAITING), pauseStartTimeMs(0), totalPauseDurationMs(0), sessionStartTimeMs(0) {}

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

    uint32_t newHostId = hostUserId;

    if (userId == hostUserId) {
        if (!participants.empty()) {
            newHostId = participants.begin()->first;
            hostUserId = newHostId;
            std::cout << "[Room " << roomId << "] Host migrated from " << userId << " to " << newHostId << std::endl;
        }
    }

    PlayerLeftNotification notif;
    notif.user_id = userId;
    notif.new_host_user_id = newHostId;
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

void Room::handleReturnToRoom(uint32_t userId) {
    std::lock_guard<std::mutex> lock(roomMutex);
    
    auto it = participants.find(userId);
    if (it == participants.end()) return;
    
    // Reset player to waiting/not ready state
    it->second.isReady = false;
    it->second.score = 0;
    it->second.isEliminated = false;
    it->second.hasAnswered = false;
    it->second.selectedOption = 0;
    it->second.lastResponseTimeMs = 0;
    it->second.lastScoreChange = 0;
    
    // Reset room to WAITING if it was FINISHED
    if (state == RoomState::FINISHED) {
        state = RoomState::WAITING;
        currentQuestionIndex = 0;
        questions.clear();
        pendingLogs.clear();
    }
    
    // Broadcast updated ready status to all players
    ReadyStatusNotification notif;
    notif.user_id = userId;
    notif.is_ready = false;
    broadcast(MessageType::S2C_READY_STATUS_NOTIF, &notif, sizeof(notif));
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
            pair.second.session->writeData();
        }
    }
}

void Room::startGame() {
    state = RoomState::STARTING;
    stateStartTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    totalPauseDurationMs = 0;
    sessionStartTimeMs = stateStartTimeMs;
    
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
            
            // Check if game should end due to elimination
            if (gameMode == GameMode::ELIMINATION) {
                int activePlayers = 0;
                for (const auto& p : participants) {
                    if (!p.second.isEliminated) activePlayers++;
                }
                
                // If 0 or 1 players left: Game ends (last survivor wins or no winner)
                if (activePlayers <= 1) {
                    finishGame();
                    return;
                }
            }
            
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
    notif.winner_count = 0;
    notif.end_reason = GameEndReason::SINGLE_WINNER;

    // Check for draw: multiple players with same highest score and not eliminated
    int activePlayers = 0;
    uint32_t maxScore = 0;
    for (const auto* p : sortedPlayers) {
        if (!p->isEliminated) {
            activePlayers++;
            if (activePlayers == 1) maxScore = p->score;
        }
    }

    // Count how many non-eliminated players have the max score (draw condition)
    int winnersCount = 0;
    if (activePlayers > 0) {
        for (const auto* p : sortedPlayers) {
            if (!p->isEliminated && p->score == maxScore) {
                winnersCount++;
            }
        }
    } else {
        // No winner - everyone eliminated (wipeout scenario)
        notif.end_reason = GameEndReason::NO_WINNER_WIPEOUT;
    }

    // Set end reason: draw if multiple winners, single winner if one, wipeout if none
    if (notif.end_reason != GameEndReason::NO_WINNER_WIPEOUT) {
        notif.end_reason = (winnersCount > 1) ? GameEndReason::DRAW : GameEndReason::SINGLE_WINNER;
    }
    notif.winner_count = winnersCount;

    std::vector<UserRepository::RankUpdateInfo> eloUpdates;

    for (size_t i = 0; i < sortedPlayers.size(); ++i) {
        PlayerFinalResult& res = notif.results[notif.result_count++];
        res.user_id = sortedPlayers[i]->session->getUserId();
        std::strncpy(res.display_name, sortedPlayers[i]->session->getDisplayName().c_str(), MAX_DISPLAY_NAME_LEN - 1);
        res.final_rank = static_cast<uint32_t>(i + 1);
        res.final_score = sortedPlayers[i]->score;
        
        // Mark as winner if in draw/single winner scenario
        if (!sortedPlayers[i]->isEliminated && sortedPlayers[i]->score == maxScore) {
            res.is_winner = true;
        } else {
            res.is_winner = false;
        }

        eloUpdates.push_back({res.user_id, res.final_rank, res.final_score});
    }

    std::cout << "[Room " << roomId << "] Game Over: end_reason=" << (int)notif.end_reason 
              << " winners=" << (int)notif.winner_count << std::endl;

    broadcast(MessageType::S2C_GAME_OVER_NOTIF, &notif, sizeof(notif));

    persistResults(sortedPlayers, TerminationReason::UNKNOWN);
    UserRepository::updateUserRanks(eloUpdates);
}

void Room::persistResults(const std::vector<PlayerGameData*>& sortedPlayers, TerminationReason reason) {
    try {
        auto& db = DatabaseManager::getInstance().getDb();
        
        db << "BEGIN TRANSACTION;";

        auto msToIso = [](uint64_t ms)->std::string {
            std::time_t t = static_cast<std::time_t>(ms/1000);
            std::tm tm{};
            gmtime_r(&t, &tm);
            char buf[50];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            return std::string(buf);
        };
        uint64_t endMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        uint64_t startMs = (sessionStartTimeMs != 0) ? sessionStartTimeMs : endMs;
        std::string startIso = msToIso(startMs);
        std::string endIso = msToIso(endMs);

        std::string modeStr = (gameMode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
        db << "INSERT INTO game_sessions (game_mode, created_at, ended_at) VALUES (?, ?, ?);" 
           << modeStr << startIso << endIso;
        
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
        std::cout << "[Room " << roomId << "] Game results persisted. Session ID: " << sessionId << std::endl;
        sessionStartTimeMs = 0;
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
```

---

##  server/network/ClientSession.cpp 

```cpp
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
        case MessageType::C2S_START_GAME_REQ:
            if (state.isAuthenticated)
                handleStartGame();
            break;
        case MessageType::C2S_SUBMIT_ANSWER_REQ:
            if (state.isAuthenticated && body.size() >= sizeof(SubmitAnswerRequest))
                handleSubmitAnswer(reinterpret_cast<const SubmitAnswerRequest*>(body.data()));
            break;
        case MessageType::C2S_RETURN_TO_ROOM_REQ:
            if (state.isAuthenticated) handleReturnToRoom();
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
        case MessageType::C2S_GET_REPLAY_REQ:
            if (body.size() >= sizeof(GetReplayRequest))
                handleGetReplay(reinterpret_cast<const GetReplayRequest*>(body.data()));
            break;
        case MessageType::C2S_GET_GAME_HISTORY_REQ:
            if (state.isAuthenticated)
                handleGetGameHistory();
            break;
        case MessageType::C2S_LEAVE_MATCH_REQ:
            if (state.isAuthenticated)
                handleForfeitGame();
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
        
        JoinRoomResponse playerListRsp{};
        playerListRsp.code = StatusCode::SUCCESS;
        playerListRsp.room_info = room->getRoomInfo();
        playerListRsp.host_user_id = room->getHostId();
        room->getPlayerList(playerListRsp);
        sendMsg(MessageType::S2C_JOIN_ROOM_RSP, &playerListRsp, sizeof(playerListRsp));
        writeData();
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

void ClientSession::handleStartGame() {
    if (state.currentRoomId == 0) return;

    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->handleStartGame(state.userId);
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

void ClientSession::handleReturnToRoom() {
    if (state.currentRoomId == 0) return;
    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (room) {
        room->handleReturnToRoom(state.userId);
        StatusResponse rsp;
        rsp.code = StatusCode::SUCCESS;
        sendResponse(MessageType::S2C_RETURN_TO_ROOM_RSP, &rsp, sizeof(rsp));
    }
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

void ClientSession::handleGetReplay(const GetReplayRequest* req) {
    try {
        auto &db = DatabaseManager::getInstance().getDb();
        
        std::vector<ReplayEvent> events;
        GameMode mode = GameMode::ELIMINATION;

        db << "SELECT game_mode FROM game_sessions WHERE id = ?"
           << req->session_id
           >> [&](const std::string& modeStr) {
               mode = (modeStr == "Elimination") ? GameMode::ELIMINATION : GameMode::SCORING;
           };

        db << "SELECT gl.timestamp, gl.question_id, gl.user_id, gl.selected_option, "
                  "gl.is_correct, gl.response_time_ms, "
                  "q.content, q.option1, q.option2, q.option3, q.option4, "
                  "q.correct_option, q.difficulty "
              "FROM game_log gl "
              "JOIN questions q ON gl.question_id = q.id "
              "WHERE gl.session_id = ? AND gl.user_id = ? "
              "ORDER BY gl.id ASC"
           << req->session_id << state.userId
           >> [&](const std::string& ts, uint32_t qid, uint32_t uid, int sel_opt,
                  bool correct, uint32_t resp_ms,
                  const std::string& content, const std::string& opt1,
                  const std::string& opt2, const std::string& opt3,
                  const std::string& opt4, int corr_opt, int diff) {
               ReplayEvent event{};
               event.timestamp_ms = resp_ms;
               event.question_id = qid;
               event.user_id = uid;
               event.selected_option = static_cast<uint8_t>(sel_opt);
               event.is_correct = correct;
               event.response_time_ms = resp_ms;
               event.correct_option = static_cast<uint8_t>(corr_opt);
               event.difficulty = static_cast<uint8_t>(diff);

               strncpy(event.question_content, content.c_str(), MAX_QUESTION_CONTENT_LEN - 1);
               event.question_content[MAX_QUESTION_CONTENT_LEN - 1] = '\0';

               strncpy(event.options[0], opt1.c_str(), MAX_OPTION_CONTENT_LEN - 1);
               event.options[0][MAX_OPTION_CONTENT_LEN - 1] = '\0';

               strncpy(event.options[1], opt2.c_str(), MAX_OPTION_CONTENT_LEN - 1);
               event.options[1][MAX_OPTION_CONTENT_LEN - 1] = '\0';

               strncpy(event.options[2], opt3.c_str(), MAX_OPTION_CONTENT_LEN - 1);
               event.options[2][MAX_OPTION_CONTENT_LEN - 1] = '\0';

               strncpy(event.options[3], opt4.c_str(), MAX_OPTION_CONTENT_LEN - 1);
               event.options[3][MAX_OPTION_CONTENT_LEN - 1] = '\0';
               
               events.push_back(event);
           };
        
        ReplayDataResponse response{};
        response.status = StatusCode::SUCCESS;
        response.session_id = req->session_id;
        response.game_mode = mode;
        response.event_count = static_cast<uint32_t>(events.size());
        
        std::vector<uint8_t> payload;
        payload.resize(sizeof(ReplayDataResponse) + events.size() * sizeof(ReplayEvent));
        
        memcpy(payload.data(), &response, sizeof(ReplayDataResponse));
        memcpy(payload.data() + sizeof(ReplayDataResponse), events.data(), events.size() * sizeof(ReplayEvent));
        
        sendResponse(MessageType::S2C_GET_REPLAY_RSP, payload.data(), payload.size());
    } catch (std::exception &e) {
        std::cerr << "DB Error getting replay: " << e.what() << std::endl;
        ReplayDataResponse errorResponse{};
        errorResponse.status = StatusCode::FAILURE_GENERIC;
        errorResponse.session_id = req->session_id;
        errorResponse.game_mode = GameMode::ELIMINATION;
        errorResponse.event_count = 0;
        sendResponse(MessageType::S2C_GET_REPLAY_RSP, &errorResponse, sizeof(errorResponse));
    }
}

void ClientSession::handleGetGameHistory() {
    GameHistoryResponse response{};
    response.status = StatusCode::SUCCESS;
    response.entry_count = 0;

    try {
        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT sp.session_id, gs.game_mode, sp.score, sp.rank, "
                  "IFNULL(SUM(CASE WHEN gl.is_correct = 1 THEN 1 ELSE 0 END), 0), "
                  "COUNT(DISTINCT gl.question_id), "
                  "IFNULL(AVG(gl.response_time_ms), 0), "
                  "unixepoch(gs.created_at) "
              "FROM session_participants sp "
              "JOIN game_sessions gs ON sp.session_id = gs.id "
              "LEFT JOIN game_log gl ON sp.session_id = gl.session_id AND sp.user_id = gl.user_id "
              "WHERE sp.user_id = ? "
              "GROUP BY sp.session_id, gs.created_at "
              "ORDER BY gs.created_at DESC "
              "LIMIT 100"
           << state.userId
           >> [&](uint32_t sid, const std::string& mode, uint32_t score, int rank,
                  int correct, int total, double avg_time, uint64_t timestamp) {
               if (response.entry_count < 100) {
                   auto& entry = response.entries[response.entry_count++];
                   entry.session_id = sid;
                   entry.player_score = score;
                   entry.player_rank = (rank > 0) ? rank : 0;
                   entry.correct_answers = correct;
                   entry.total_questions = total;
                   entry.avg_response_time_ms = static_cast<uint32_t>(avg_time);
                   entry.timestamp_sec = timestamp;
                   
                   strncpy(entry.game_mode, mode.c_str(), sizeof(entry.game_mode) - 1);
                   entry.game_mode[sizeof(entry.game_mode) - 1] = '\0';
               }
           };

        sendResponse(MessageType::S2C_GET_GAME_HISTORY_RSP, &response, sizeof(response));
    } catch (const std::exception &e) {
        std::cerr << "DB Error getting game history: " << e.what() << std::endl;
        response.status = StatusCode::FAILURE_GENERIC;
        sendResponse(MessageType::S2C_GET_GAME_HISTORY_RSP, &response, sizeof(response));
    }
}

void ClientSession::handleForfeitGame() {
    if (state.currentRoomId == 0) return;
    
    Room* room = server->getRoomManager()->getRoom(state.currentRoomId);
    if (!room) return;
    
    const int32_t ELO_PENALTY = 100;
    
    try {
        auto& db = DatabaseManager::getInstance().getDb();
        
        db << "UPDATE users SET ranked_points = MAX(0, ranked_points - ?) WHERE id = ?"
           << ELO_PENALTY << state.userId;
        
        std::cout << "Player " << state.displayName << " forfeited game. ELO penalty: -" 
                  << ELO_PENALTY << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "DB Error applying forfeit penalty: " << e.what() << std::endl;
    }
    
    GameTerminatedNotification notif{};
    notif.reason = TerminationReason::HOST_LEFT; 
    sendResponse(MessageType::S2C_GAME_TERMINATED_NOTIF, &notif, sizeof(notif));
    
    handleLeaveRoom();
}
```

---

##  server/network/Server.h 

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

##  server/network/Server.cpp 

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
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, 50);

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
        roomManager->updateAllRooms();
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

##  server/network/ClientSession.h 

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
    void handleStartGame();
    void handleSubmitAnswer(const SubmitAnswerRequest* req);
    void handleReturnToRoom();
    void handleGetStats();
    void handlePauseGame();
    void handleResumeGame();
    void handleGetReplay(const GetReplayRequest* req);
    void handleGetGameHistory();
    void handleForfeitGame();

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

##  server/utils/PasswordUtils.h 

```cpp
#include <iostream>

```

---

##  server/utils/Logger.h 

```cpp
#include <iostream>

```

---

