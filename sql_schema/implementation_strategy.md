## **Phase 1: Foundation & Core Infrastructure (Week 1-2)**

### **Step 1: CMake Build System & Dependencies**
```bash
# Start here - this enables everything else
server/CMakeLists.txt
```
- Configure CMake to include:
  - pthread for threading
  - SQLite3 library linking
  - C++17 standard
  - Proper include directories for `thirdparty/`
- Create build targets for development and release modes
- **Priority: CRITICAL** - Without this, you can't compile anything

### **Step 2: Database Initialization & Schema**
```bash
# Essential foundation for all persistent data
server/database_init.cpp
server/database_schema.sql
```
Create tables based on your protocol:
```sql
-- users table
CREATE TABLE users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    email TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    display_name TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- rooms table  
CREATE TABLE rooms (
    room_id INTEGER PRIMARY KEY AUTOINCREMENT,
    room_name TEXT NOT NULL,
    game_mode INTEGER NOT NULL, -- 0=ELIMINATION, 1=SCORING
    num_questions INTEGER NOT NULL,
    host_user_id INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(host_user_id) REFERENCES users(user_id)
);

-- matches table (for statistics)
CREATE TABLE matches (
    match_id INTEGER PRIMARY KEY AUTOINCREMENT,
    room_id INTEGER NOT NULL,
    start_time TIMESTAMP NOT NULL,
    end_time TIMESTAMP,
    FOREIGN KEY(room_id) REFERENCES rooms(room_id)
);
```
- **Priority: HIGH** - All user/game data depends on this

### **Step 3: Basic TCP Server Skeleton**
```cpp
// server/server.cpp (start with minimal version)
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>

class TCPServer {
private:
    int server_fd_;
    sockaddr_in address_;
    std::vector<std::thread> client_threads_;
    std::mutex clients_mutex_;

public:
    bool init(uint16_t port);
    void start();
    void handle_client(int client_socket);
};

// Minimal main() to test server startup
int main() {
    TCPServer server;
    if (!server.init(8080)) {
        std::cerr << "Server initialization failed" << std::endl;
        return 1;
    }
    server.start();
    return 0;
}
```
- **Priority: CRITICAL** - Core networking foundation
- Implement proper error handling and graceful shutdown
- Start with single-threaded version, then add threading

## **Phase 2: Communication Protocol (Week 2-3)**

### **Step 4: Message Serialization/Deserialization**
```bash
server/message_handler.cpp
server/message_handler.h
```
```cpp
// Based on your protocol_h.txt
class MessageHandler {
public:
    // Deserialize incoming raw bytes to structured messages
    bool deserialize_message(const uint8_t* buffer, size_t len, 
                           MessageHeader& header, void*& body);
    
    // Serialize structured messages to raw bytes
    std::vector<uint8_t> serialize_message(MessageType type, 
                                          const void* body, size_t body_len);
    
    // Validate message integrity
    bool validate_message(const MessageHeader& header, const void* body);
};
```
- **Priority: HIGH** - Required for all client-server communication
- Implement endian conversion (network byte order)
- Add message length validation against protocol definitions

### **Step 5: Connection Manager**
```bash
server/connection_manager.cpp
server/connection_manager.h
```
```cpp
class Connection {
private:
    int socket_fd_;
    uint32_t user_id_; // 0 if not authenticated
    bool authenticated_;
    std::mutex send_mutex_;

public:
    void send_message(const std::vector<uint8_t>& message);
    bool receive_message(MessageHeader& header, void*& body);
    void set_authenticated(uint32_t user_id);
};

class ConnectionManager {
private:
    std::unordered_map<int, std::unique_ptr<Connection>> connections_;
    std::mutex connections_mutex_;

public:
    Connection* add_connection(int socket_fd);
    void remove_connection(int socket_fd);
    void broadcast_to_room(uint32_t room_id, const std::vector<uint8_t>& message);
};
```
- **Priority: HIGH** - Manages all client connections
- Implement timeout handling (30s for authentication, 5min for inactive connections)
- Add thread-safe operations for concurrent access

## **Phase 3: Authentication System (Week 3-4)**

### **Step 6: User Authentication Handlers**
```bash
server/auth_handler.cpp
server/auth_handler.h
```
```cpp
class AuthHandler {
private:
    DatabaseInterface& db_;

public:
    void handle_register_request(Connection* conn, const RegisterRequest& req);
    void handle_login_request(Connection* conn, const LoginRequest& req);
    void handle_account_update_request(Connection* conn, 
                                     const AccountUpdateRequest& req,
                                     uint32_t user_id);
};
```
- **Priority: HIGH** - Blocks all other functionality
- Implement password hashing (bcrypt or SHA-256 with salt)
- Add rate limiting to prevent brute force attacks
- Validate email format and display name constraints

### **Step 7: Session Management**
```bash
server/session_manager.cpp
server/session_manager.h
```
```cpp
class SessionManager {
private:
    std::unordered_map<uint32_t, Connection*> user_connections_;
    std::unordered_map<uint32_t, time_t> last_activity_;
    std::mutex sessions_mutex_;

public:
    bool login_user(uint32_t user_id, Connection* conn);
    void logout_user(uint32_t user_id);
    Connection* get_connection(uint32_t user_id);
    void cleanup_inactive_sessions(); // Run in background thread
};
```
- **Priority: MEDIUM-HIGH** - Required for stateful interactions
- Implement session timeout (15-30 minutes of inactivity)
- Add cleanup thread that runs periodically

## **Phase 4: Room Management (Week 4-5)**

### **Step 8: Room System Core**
```bash
server/room_manager.cpp
server/room_manager.h
```
```cpp
struct RoomPlayer {
    uint32_t user_id;
    std::string display_name;
    bool is_ready;
    bool is_host;
};

class Room {
private:
    uint32_t room_id_;
    std::string room_name_;
    GameMode game_mode_;
    uint8_t max_players_;
    bool is_game_started_;
    std::vector<RoomPlayer> players_;
    std::mutex room_mutex_;

public:
    bool add_player(const RoomPlayer& player);
    bool remove_player(uint32_t user_id);
    void set_ready_status(uint32_t user_id, bool is_ready);
    bool can_start_game() const;
};

class RoomManager {
private:
    std::unordered_map<uint32_t, std::unique_ptr<Room>> rooms_;
    std::mutex rooms_mutex_;

public:
    uint32_t create_room(const CreateRoomRequest& req, uint32_t host_user_id);
    bool join_room(uint32_t room_id, uint32_t user_id, const std::string& display_name);
    std::vector<RoomInfo> get_room_list();
};
```
- **Priority: MEDIUM-HIGH** - Core game functionality
- Implement room cleanup for empty/finished rooms
- Add automatic game start when ready (timer-based)

### **Step 9: Room Request Handlers**
```bash
server/room_handler.cpp
server/room_handler.h
```
```cpp
class RoomHandler {
private:
    RoomManager& room_manager_;
    ConnectionManager& conn_manager_;

public:
    void handle_list_rooms_request(Connection* conn);
    void handle_create_room_request(Connection* conn, 
                                  const CreateRoomRequest& req,
                                  uint32_t user_id);
    void handle_join_room_request(Connection* conn, 
                                const JoinRoomRequest& req,
                                uint32_t user_id);
    void handle_leave_room_request(Connection* conn, uint32_t user_id);
    void handle_ready_status_request(Connection* conn, 
                                   const ReadyStatusRequest& req,
                                   uint32_t user_id);
};
```
- **Priority: MEDIUM-HIGH** - User-facing room functionality
- Implement notifications for player joins/leaves
- Add validation for room state transitions

## **Phase 5: Game Logic (Week 5-7)**

### **Step 10: Game State Manager**
```bash
server/game_manager.cpp
server/game_manager.h
```
```cpp
class Game {
private:
    uint32_t room_id_;
    GameMode mode_;
    std::vector<uint32_t> question_ids_;
    uint8_t current_question_idx_;
    std::unordered_map<uint32_t, SubmitAnswerRequest> player_answers_;
    std::unordered_map<uint32_t, int32_t> player_scores_;
    bool is_active_;
    std::mutex game_mutex_;

public:
    void start_game();
    void process_answer(uint32_t user_id, const SubmitAnswerRequest& answer);
    void calculate_round_results();
    void end_game();
};

class GameManager {
private:
    std::unordered_map<uint32_t, std::unique_ptr<Game>> active_games_;
    std::mutex games_mutex_;

public:
    void start_game_for_room(uint32_t room_id);
    void handle_submit_answer(uint32_t room_id, uint32_t user_id, 
                            const SubmitAnswerRequest& req);
};
```
- **Priority: MEDIUM** - Core game experience
- Implement timer management for question rounds
- Add elimination logic for elimination mode
- Implement scoring algorithm for scoring mode

### **Step 11: Question Management**
```bash
server/question_manager.cpp
server/question_manager.h
```
```cpp
struct Question {
    uint32_t question_id;
    std::string content;
    std::array<std::string, 4> options;
    uint8_t correct_option;
    uint8_t time_limit_sec;
    uint16_t difficulty;
};

class QuestionManager {
private:
    DatabaseInterface& db_;
    std::vector<Question> question_bank_;

public:
    void load_questions();
    const Question& get_random_question(uint16_t difficulty = 0);
    bool validate_answer(uint32_t question_id, uint8_t selected_option);
};
```
- **Priority: MEDIUM** - Game content delivery
- Implement question difficulty progression
- Add caching for frequently accessed questions

## **Phase 6: Statistics & Advanced Features (Week 7-8)**

### **Step 12: Statistics Manager**
```bash
server/stats_manager.cpp
server/stats_manager.h
```
```cpp
class StatsManager {
private:
    DatabaseInterface& db_;

public:
    void record_match_result(uint32_t match_id, uint32_t room_id,
                           const std::vector<PlayerFinalResult>& results);
    
    bool get_user_stats(uint32_t user_id, UserStatsResponse& response);
    
    void record_question_answer(uint32_t user_id, uint32_t question_id,
                              bool is_correct, uint32_t response_time_ms);
};
```
- **Priority: LOW-MEDIUM** - Post-MVP feature
- Implement efficient aggregation queries
- Add caching for frequently requested stats

### **Step 13: Error Handling & Logging**
```bash
server/error_handler.cpp
server/logger.cpp
```
```cpp
class ErrorHandler {
public:
    static std::vector<uint8_t> create_error_response(StatusCode code, 
                                                    const char* message = nullptr);
    
    static void log_error(const std::string& component, 
                         const std::string& error_message,
                         int error_code = 0);
};

class Logger {
public:
    static void init(const std::string& log_file);
    static void log_info(const std::string& message);
    static void log_warning(const std::string& message);
    static void log_error(const std::string& message);
};
```
- **Priority: MEDIUM** - Critical for debugging and production
- Implement structured logging with timestamps
- Add log rotation to prevent disk space issues

## **Phase 7: Integration & Testing (Week 8+)**

### **Step 14: Main Server Integration**
```cpp
// Final server.cpp integration
#include "connection_manager.h"
#include "auth_handler.h"
#include "room_handler.h"
#include "game_manager.h"
#include "stats_manager.h"
#include "logger.h"

int main() {
    Logger::init("server.log");
    
    // Initialize database
    DatabaseInterface db;
    if (!db.init("game.db")) {
        Logger::log_error("Database initialization failed");
        return 1;
    }
    
    // Initialize managers
    ConnectionManager conn_mgr;
    AuthHandler auth_handler(db);
    RoomManager room_mgr;
    RoomHandler room_handler(room_mgr, conn_mgr);
    GameManager game_mgr;
    StatsManager stats_mgr(db);
    
    // Start TCP server
    TCPServer server(conn_mgr, auth_handler, room_handler, game_mgr, stats_mgr);
    if (!server.init(8080)) {
        Logger::log_error("Server initialization failed");
        return 1;
    }
    
    Logger::log_info("Server started on port 8080");
    server.start();
    
    return 0;
}
```

## **Critical Implementation Order Summary:**

1. **Foundation First** (Week 1):
   - `CMakeLists.txt` → `database_init.cpp` → Basic `server.cpp`

2. **Communication Core** (Week 2):
   - Message serialization → Connection management

3. **User Flow** (Week 3-4):
   - Authentication handlers → Session management → Room system

4. **Game Logic** (Week 5-7):
   - Game state manager → Question system → Scoring/elimination logic

5. **Polish & Production** (Week 8+):
   - Statistics → Error handling → Logging → Integration testing

## **Key Dependencies to Remember:**
- **Database schema** must be designed before any handler implementation
- **Message serialization** must work before any protocol handling
- **Authentication** must be complete before room/game features
- **Room management** must be stable before game logic implementation
- **Error handling** should be integrated early but can be refined later

## **Testing Strategy:**
1. Start with unit tests for message serialization
2. Test database operations in isolation
3. Create mock clients for integration testing
4. Test authentication flow end-to-end
5. Test room creation/joining scenarios
6. Test full game flow with 2-3 mock players

This phased approach ensures you have a working foundation before adding complexity, making debugging much more manageable and allowing for incremental feature delivery.