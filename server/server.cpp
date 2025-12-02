#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "../common/protocol.h"
#include <sqlite_modern_cpp.h>

// Database path
const std::string DB_PATH = "build/game_db.db";

// Forward declarations
class Database;
class SessionManager;
class RoomManager;
class GameManager;

// Utility functions
std::string hash_password(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, password.c_str(), password.length());
    EVP_DigestFinal_ex(mdctx, hash, NULL);
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool verify_password(const std::string& password, const std::string& hash) {
    return hash_password(password) == hash;
}

// Database wrapper
class Database {
private:
    sqlite::database db_;

public:
    Database(const std::string& path) : db_(path) {}

    sqlite::database& get() { return db_; }

    bool register_user(const std::string& email, const std::string& display_name, const std::string& password) {
        try {
            std::string hashed = hash_password(password);
            db_ << "INSERT INTO users (email, display_name, hashed_password) VALUES (?, ?, ?)"
                << email << display_name << hashed;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Registration error: " << e.what() << std::endl;
            return false;
        }
    }

    bool check_email_exists(const std::string& email) {
        int count = 0;
        db_ << "SELECT COUNT(*) FROM users WHERE email = ?" << email >> count;
        return count > 0;
    }

    bool authenticate_user(const std::string& email, const std::string& password, uint32_t& user_id, std::string& display_name) {
        try {
            std::string stored_hash;
            db_ << "SELECT id, display_name, hashed_password FROM users WHERE email = ?" << email
                >> [&](int id, std::string name, std::string hash) {
                    stored_hash = hash;
                    user_id = id;
                    display_name = name;
                };
            
            if (stored_hash.empty()) return false;
            return verify_password(password, stored_hash);
        } catch (const std::exception& e) {
            return false;
        }
    }

    bool update_display_name(uint32_t user_id, const std::string& new_name) {
        try {
            db_ << "UPDATE users SET display_name = ? WHERE id = ?" << new_name << user_id;
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }

    std::vector<RoomInfo> get_available_rooms() {
        std::vector<RoomInfo> rooms;
        // This will be implemented with RoomManager
        return rooms;
    }

    void save_game_session(uint32_t session_id, const std::string& game_mode) {
        try {
            db_ << "INSERT INTO game_sessions (id, game_mode, created_at) VALUES (?, ?, CURRENT_TIMESTAMP)"
                << session_id << game_mode;
        } catch (const std::exception& e) {
            std::cerr << "Error saving game session: " << e.what() << std::endl;
        }
    }

    void save_participant_result(uint32_t session_id, uint32_t user_id, int32_t score, int32_t rank) {
        try {
            db_ << "INSERT INTO session_participants (session_id, user_id, score, rank) VALUES (?, ?, ?, ?)"
                << session_id << user_id << score << rank;
        } catch (const std::exception& e) {
            std::cerr << "Error saving participant result: " << e.what() << std::endl;
        }
    }

    void save_answer_log(uint32_t session_id, uint32_t user_id, uint32_t question_id, 
                        uint8_t selected_option, bool is_correct, uint32_t response_time_ms) {
        try {
            db_ << "INSERT INTO game_log (session_id, user_id, question_id, selected_option, is_correct, response_time_ms) "
                   "VALUES (?, ?, ?, ?, ?, ?)"
                << session_id << user_id << question_id << selected_option << is_correct << response_time_ms;
        } catch (const std::exception& e) {
            std::cerr << "Error saving answer log: " << e.what() << std::endl;
        }
    }

    UserStatsResponse get_user_stats(uint32_t user_id) {
        UserStatsResponse stats = {0};
        try {
            db_ << "SELECT COUNT(*) FROM session_participants WHERE user_id = ?" << user_id >> stats.total_matches;
            
            // Count wins (rank = 1)
            db_ << "SELECT COUNT(*) FROM session_participants WHERE user_id = ? AND rank = 1" 
                << user_id >> stats.wins;
            
            // Count correct/incorrect answers
            db_ << "SELECT COUNT(*) FROM game_log WHERE user_id = ? AND is_correct = 1" 
                << user_id >> stats.total_correct_answers;
            db_ << "SELECT COUNT(*) FROM game_log WHERE user_id = ? AND is_correct = 0" 
                << user_id >> stats.total_incorrect_answers;
            
            // Average score
            double avg = 0.0;
            db_ << "SELECT AVG(score) FROM session_participants WHERE user_id = ?" 
                << user_id >> avg;
            stats.average_score = avg;
        } catch (const std::exception& e) {
            std::cerr << "Error getting stats: " << e.what() << std::endl;
        }
        return stats;
    }

    std::vector<QuestionNotification> get_random_questions(uint8_t num_questions) {
        std::vector<QuestionNotification> questions;
        try {
            int count = 0;
            db_ << "SELECT COUNT(*) FROM questions" >> count;
            if (count == 0) {
                std::cerr << "Warning: No questions in database!" << std::endl;
                return questions;
            }
            
            db_ << "SELECT id, content, option1, option2, option3, option4, correct_option FROM questions "
                   "ORDER BY RANDOM() LIMIT ?" << num_questions
                >> [&](int id, std::string content, std::string opt1, std::string opt2, 
                       std::string opt3, std::string opt4, int correct) {
                    QuestionNotification q;
                    q.question_id = id;
                    q.time_limit_sec = 30; // Default 30 seconds
                    strncpy(q.content, content.c_str(), MAX_QUESTION_CONTENT_LEN - 1);
                    q.content[MAX_QUESTION_CONTENT_LEN - 1] = '\0';
                    strncpy(q.options[0], opt1.c_str(), MAX_OPTION_CONTENT_LEN - 1);
                    q.options[0][MAX_OPTION_CONTENT_LEN - 1] = '\0';
                    strncpy(q.options[1], opt2.c_str(), MAX_OPTION_CONTENT_LEN - 1);
                    q.options[1][MAX_OPTION_CONTENT_LEN - 1] = '\0';
                    strncpy(q.options[2], opt3.c_str(), MAX_OPTION_CONTENT_LEN - 1);
                    q.options[2][MAX_OPTION_CONTENT_LEN - 1] = '\0';
                    strncpy(q.options[3], opt4.c_str(), MAX_OPTION_CONTENT_LEN - 1);
                    q.options[3][MAX_OPTION_CONTENT_LEN - 1] = '\0';
                    questions.push_back(q);
                };
        } catch (const std::exception& e) {
            std::cerr << "Error getting questions: " << e.what() << std::endl;
        }
        return questions;
    }
    
    uint8_t get_correct_option(uint32_t question_id) {
        uint8_t correct = 0;
        try {
            db_ << "SELECT correct_option FROM questions WHERE id = ?" << question_id >> correct;
        } catch (const std::exception& e) {
            std::cerr << "Error getting correct option: " << e.what() << std::endl;
        }
        return correct;
    }
};


// Session management
struct Session {
    uint32_t user_id;
    int socket_fd;
    std::string display_name;
    bool is_authenticated;
    uint32_t current_room_id;
    bool is_ready;
    
    Session() : user_id(0), socket_fd(-1), is_authenticated(false), 
                current_room_id(0), is_ready(false) {}
};

class SessionManager {
private:
    std::mutex mutex_;
    std::unordered_map<int, Session> sessions_; // socket_fd -> Session

public:
    Session* get_session(int socket_fd) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(socket_fd);
        if (it != sessions_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    Session* create_session(int socket_fd) {
        std::lock_guard<std::mutex> lock(mutex_);
        Session& session = sessions_[socket_fd];
        session.socket_fd = socket_fd;
        return &session;
    }

    void remove_session(int socket_fd) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.erase(socket_fd);
    }

    std::vector<Session*> get_all_sessions() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Session*> result;
        for (auto& pair : sessions_) {
            if (pair.second.is_authenticated) {
                result.push_back(&pair.second);
            }
        }
        return result;
    }
};

// Room management
struct Room {
    uint32_t room_id;
    std::string room_name;
    GameMode game_mode;
    uint8_t num_questions;
    uint32_t host_user_id;
    std::vector<uint32_t> player_ids;
    std::unordered_map<uint32_t, bool> player_ready;
    bool is_in_game;
    uint32_t current_question_index;
    std::vector<QuestionNotification> questions;
    std::unordered_map<uint32_t, int32_t> player_scores; // user_id -> score
    std::unordered_map<uint32_t, bool> player_eliminated; // user_id -> eliminated
    
    Room() : room_id(0), game_mode(GameMode::ELIMINATION), num_questions(10),
             host_user_id(0), is_in_game(false), current_question_index(0) {}
};

class RoomManager {
private:
    std::mutex mutex_;
    std::unordered_map<uint32_t, Room> rooms_;
    uint32_t next_room_id_;

public:
    RoomManager() : next_room_id_(1) {}

    uint32_t create_room(const std::string& room_name, GameMode mode, uint8_t num_questions, uint32_t host_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        Room room;
        room.room_id = next_room_id_++;
        room.room_name = room_name;
        room.game_mode = mode;
        room.num_questions = num_questions;
        room.host_user_id = host_id;
        room.player_ids.push_back(host_id);
        room.player_ready[host_id] = false;
        rooms_[room.room_id] = room;
        return room.room_id;
    }

    Room* get_room(uint32_t room_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = rooms_.find(room_id);
        if (it != rooms_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    bool join_room(uint32_t room_id, uint32_t user_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = rooms_.find(room_id);
        if (it == rooms_.end() || it->second.is_in_game) {
            return false;
        }
        if (it->second.player_ids.size() >= MAX_PLAYERS_PER_ROOM) {
            return false;
        }
        it->second.player_ids.push_back(user_id);
        it->second.player_ready[user_id] = false;
        return true;
    }

    void leave_room(uint32_t room_id, uint32_t user_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = rooms_.find(room_id);
        if (it != rooms_.end()) {
            auto& player_ids = it->second.player_ids;
            player_ids.erase(std::remove(player_ids.begin(), player_ids.end(), user_id), player_ids.end());
            it->second.player_ready.erase(user_id);
            
            // If host left, assign new host
            if (it->second.host_user_id == user_id && !player_ids.empty()) {
                it->second.host_user_id = player_ids[0];
            }
        }
    }

    std::vector<RoomInfo> get_available_rooms() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<RoomInfo> result;
        for (const auto& pair : rooms_) {
            const Room& room = pair.second;
            if (!room.is_in_game && room.player_ids.size() < MAX_PLAYERS_PER_ROOM) {
                RoomInfo info;
                info.room_id = room.room_id;
                strncpy(info.room_name, room.room_name.c_str(), MAX_ROOM_NAME_LEN - 1);
                info.current_players = room.player_ids.size();
                info.max_players = MAX_PLAYERS_PER_ROOM;
                info.game_mode = room.game_mode;
                info.is_in_game = room.is_in_game;
                result.push_back(info);
            }
        }
        return result;
    }

    void delete_room(uint32_t room_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        rooms_.erase(room_id);
    }
};

// Global instances
Database* g_db = nullptr;
SessionManager* g_session_mgr = nullptr;
RoomManager* g_room_mgr = nullptr;

// Message sending helpers
bool send_message(int socket_fd, MessageType type, const void* body, size_t body_size) {
    MessageHeader header;
    header.type = type;
    header.body_len = body_size;

    if (send(socket_fd, &header, sizeof(header), 0) < 0) {
        return false;
    }
    if (body_size > 0 && send(socket_fd, body, body_size, 0) < 0) {
        return false;
    }
    return true;
}

bool send_error(int socket_fd, StatusCode code, const std::string& message) {
    ErrorResponse error;
    error.code = code;
    strncpy(error.message, message.c_str(), MAX_ERROR_MSG_LEN - 1);
    return send_message(socket_fd, MessageType::S2C_ERROR_RSP, &error, sizeof(error));
}

// Request handlers
void handle_register(int socket_fd, const RegisterRequest& req) {
    StatusResponse resp;
    
    if (g_db->check_email_exists(req.email)) {
        resp.code = StatusCode::EMAIL_EXISTS;
    } else {
        if (g_db->register_user(req.email, req.display_name, req.password)) {
            resp.code = StatusCode::SUCCESS;
        } else {
            resp.code = StatusCode::FAILURE_GENERIC;
        }
    }
    
    send_message(socket_fd, MessageType::S2C_REGISTER_RSP, &resp, sizeof(resp));
}

void handle_login(int socket_fd, const LoginRequest& req) {
    LoginResponse resp;
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session) {
        session = g_session_mgr->create_session(socket_fd);
    }

    uint32_t user_id;
    std::string display_name;
    if (g_db->authenticate_user(req.email, req.password, user_id, display_name)) {
        session->user_id = user_id;
        session->display_name = display_name;
        session->is_authenticated = true;
        
        resp.code = StatusCode::SUCCESS;
        resp.user_id = user_id;
        strncpy(resp.display_name, display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
    } else {
        resp.code = StatusCode::INVALID_CREDENTIALS;
    }
    
    send_message(socket_fd, MessageType::S2C_LOGIN_RSP, &resp, sizeof(resp));
}

void handle_account_update(int socket_fd, const AccountUpdateRequest& req) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not authenticated");
        return;
    }

    StatusResponse resp;
    if (g_db->update_display_name(session->user_id, req.new_display_name)) {
        session->display_name = req.new_display_name;
        resp.code = StatusCode::SUCCESS;
    } else {
        resp.code = StatusCode::FAILURE_GENERIC;
    }
    
    send_message(socket_fd, MessageType::S2C_ACCOUNT_UPDATE_RSP, &resp, sizeof(resp));
}

void handle_list_rooms(int socket_fd) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not authenticated");
        return;
    }

    ListRoomsResponse resp;
    std::vector<RoomInfo> rooms = g_room_mgr->get_available_rooms();
    resp.room_count = std::min(rooms.size(), (size_t)MAX_ROOMS_PER_LIST);
    for (size_t i = 0; i < resp.room_count; i++) {
        resp.rooms[i] = rooms[i];
    }
    
    send_message(socket_fd, MessageType::S2C_LIST_ROOMS_RSP, &resp, sizeof(resp));
}

void handle_create_room(int socket_fd, const CreateRoomRequest& req) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not authenticated");
        return;
    }

    CreateRoomResponse resp;
    uint32_t room_id = g_room_mgr->create_room(req.room_name, req.game_mode, req.num_questions, session->user_id);
    
    Room* room = g_room_mgr->get_room(room_id);
    if (room) {
        session->current_room_id = room_id;
        resp.code = StatusCode::SUCCESS;
        resp.room_info.room_id = room->room_id;
        strncpy(resp.room_info.room_name, room->room_name.c_str(), MAX_ROOM_NAME_LEN - 1);
        resp.room_info.current_players = room->player_ids.size();
        resp.room_info.max_players = MAX_PLAYERS_PER_ROOM;
        resp.room_info.game_mode = room->game_mode;
        resp.room_info.is_in_game = room->is_in_game;
    } else {
        resp.code = StatusCode::FAILURE_GENERIC;
    }
    
    send_message(socket_fd, MessageType::S2C_CREATE_ROOM_RSP, &resp, sizeof(resp));
}

void handle_join_room(int socket_fd, const JoinRoomRequest& req) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not authenticated");
        return;
    }

    JoinRoomResponse resp;
    Room* room = g_room_mgr->get_room(req.room_id);
    
    if (!room) {
        resp.code = StatusCode::ROOM_NOT_FOUND;
    } else if (room->is_in_game) {
        resp.code = StatusCode::GAME_IN_PROGRESS;
    } else if (room->player_ids.size() >= MAX_PLAYERS_PER_ROOM) {
        resp.code = StatusCode::ROOM_FULL;
    } else {
        if (g_room_mgr->join_room(req.room_id, session->user_id)) {
            session->current_room_id = req.room_id;
            resp.code = StatusCode::SUCCESS;
            resp.room_info.room_id = room->room_id;
            strncpy(resp.room_info.room_name, room->room_name.c_str(), MAX_ROOM_NAME_LEN - 1);
            resp.room_info.current_players = room->player_ids.size();
            resp.room_info.max_players = MAX_PLAYERS_PER_ROOM;
            resp.room_info.game_mode = room->game_mode;
            resp.room_info.is_in_game = room->is_in_game;
            resp.host_user_id = room->host_user_id;
            
            // Fill player info
            resp.player_count = 0;
            for (uint32_t player_id : room->player_ids) {
                Session* player_session = nullptr;
                for (auto* s : g_session_mgr->get_all_sessions()) {
                    if (s->user_id == player_id) {
                        player_session = s;
                        break;
                    }
                }
                if (player_session) {
                    resp.players[resp.player_count].user_id = player_id;
                    strncpy(resp.players[resp.player_count].display_name, 
                           player_session->display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
                    resp.players[resp.player_count].is_ready = room->player_ready[player_id];
                    resp.player_count++;
                }
            }
            
            // Notify other players
            PlayerInfo new_player;
            new_player.user_id = session->user_id;
            strncpy(new_player.display_name, session->display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
            new_player.is_ready = false;
            
            for (uint32_t player_id : room->player_ids) {
                if (player_id != session->user_id) {
                    Session* other_session = nullptr;
                    for (auto* s : g_session_mgr->get_all_sessions()) {
                        if (s->user_id == player_id) {
                            other_session = s;
                            break;
                        }
                    }
                    if (other_session) {
                        send_message(other_session->socket_fd, MessageType::S2C_PLAYER_JOINED_NOTIF, 
                                   &new_player, sizeof(new_player));
                    }
                }
            }
        } else {
            resp.code = StatusCode::FAILURE_GENERIC;
        }
    }
    
    send_message(socket_fd, MessageType::S2C_JOIN_ROOM_RSP, &resp, sizeof(resp));
}

void handle_ready_status(int socket_fd, const ReadyStatusRequest& req) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated || session->current_room_id == 0) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not in a room");
        return;
    }

    Room* room = g_room_mgr->get_room(session->current_room_id);
    if (room) {
        room->player_ready[session->user_id] = req.is_ready;
        session->is_ready = req.is_ready;
        
        // Notify all players in room
        ReadyStatusNotification notif;
        notif.user_id = session->user_id;
        notif.is_ready = req.is_ready;
        
        for (uint32_t player_id : room->player_ids) {
            Session* player_session = nullptr;
            for (auto* s : g_session_mgr->get_all_sessions()) {
                if (s->user_id == player_id) {
                    player_session = s;
                    break;
                }
            }
            if (player_session) {
                send_message(player_session->socket_fd, MessageType::S2C_READY_STATUS_NOTIF, 
                           &notif, sizeof(notif));
            }
        }
        
        // Check if all players are ready and start game
        bool all_ready = true;
        for (uint32_t player_id : room->player_ids) {
            if (!room->player_ready[player_id]) {
                all_ready = false;
                break;
            }
        }
        
        if (all_ready && room->player_ids.size() >= 2 && !room->is_in_game) {
            // Start game
            room->is_in_game = true;
            room->questions = g_db->get_random_questions(room->num_questions);
            room->current_question_index = 0;
            // Initialize scores and elimination status
            for (uint32_t player_id : room->player_ids) {
                room->player_scores[player_id] = 0;
                room->player_eliminated[player_id] = false;
            }
            
            GameStartNotification start_notif;
            start_notif.player_count = 0;
            for (uint32_t player_id : room->player_ids) {
                Session* player_session = nullptr;
                for (auto* s : g_session_mgr->get_all_sessions()) {
                    if (s->user_id == player_id) {
                        player_session = s;
                        break;
                    }
                }
                if (player_session) {
                    start_notif.players[start_notif.player_count].user_id = player_id;
                    strncpy(start_notif.players[start_notif.player_count].display_name,
                           player_session->display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
                    start_notif.players[start_notif.player_count].is_ready = true;
                    start_notif.player_count++;
                }
            }
            
            // Send start notification to all players
            for (uint32_t player_id : room->player_ids) {
                Session* player_session = nullptr;
                for (auto* s : g_session_mgr->get_all_sessions()) {
                    if (s->user_id == player_id) {
                        player_session = s;
                        break;
                    }
                }
                if (player_session) {
                    send_message(player_session->socket_fd, MessageType::S2C_GAME_START_NOTIF,
                               &start_notif, sizeof(start_notif));
                }
            }
            
            // Start game thread
            std::thread game_thread(run_game_loop, room);
            game_thread.detach();
        }
    }
}

void handle_get_stats(int socket_fd) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not authenticated");
        return;
    }

    UserStatsResponse stats = g_db->get_user_stats(session->user_id);
    send_message(socket_fd, MessageType::S2C_GET_STATS_RSP, &stats, sizeof(stats));
}

void handle_submit_answer(int socket_fd, const SubmitAnswerRequest& req) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated || session->current_room_id == 0) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Not in a game");
        return;
    }

    Room* room = g_room_mgr->get_room(session->current_room_id);
    if (!room || !room->is_in_game) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "Game not active");
        return;
    }

    if (room->current_question_index >= room->questions.size()) {
        send_error(socket_fd, StatusCode::INVALID_REQUEST, "No active question");
        return;
    }

    QuestionNotification& current_q = room->questions[room->current_question_index];
    uint8_t correct_option = g_db->get_correct_option(current_q.question_id);
    bool is_correct = (req.selected_option == correct_option);
    
    // Save answer log
    g_db->save_answer_log(room->room_id, session->user_id, req.question_id, 
                          req.selected_option, is_correct, req.response_time_ms);
    
    // Calculate score based on game mode
    int32_t score_change = 0;
    uint32_t points_for_question = 0;
    
    if (room->game_mode == GameMode::SCORING) {
        // Scoring mode: points based on correctness and speed
        if (is_correct) {
            // Base points + speed bonus (faster = more points)
            points_for_question = 100 + std::max(0, (int32_t)(100 - req.response_time_ms / 100));
            score_change = points_for_question;
            room->player_scores[session->user_id] += score_change;
        }
    } else {
        // Elimination mode
        if (!is_correct) {
            room->player_eliminated[session->user_id] = true;
            // Notify elimination
            PlayerEliminatedNotification elim_notif;
            elim_notif.user_id = session->user_id;
            elim_notif.session_id = room->room_id;
            
            for (uint32_t player_id : room->player_ids) {
                Session* player_session = nullptr;
                for (auto* s : g_session_mgr->get_all_sessions()) {
                    if (s->user_id == player_id) {
                        player_session = s;
                        break;
                    }
                }
                if (player_session) {
                    send_message(player_session->socket_fd, MessageType::S2C_PLAYER_ELIMINATED_NOTIF,
                               &elim_notif, sizeof(elim_notif));
                }
            }
        }
    }
    
    // Check if round is complete (all players answered or eliminated)
    // This would typically be handled by a game state machine
    // For now, we'll send round result after a timeout or when all answered
}

void handle_leave_room(int socket_fd) {
    Session* session = g_session_mgr->get_session(socket_fd);
    if (!session || !session->is_authenticated || session->current_room_id == 0) {
        return;
    }

    Room* room = g_room_mgr->get_room(session->current_room_id);
    if (room) {
        // Notify other players
        PlayerLeftNotification notif;
        notif.user_id = session->user_id;
        notif.new_host_user_id = room->host_user_id;
        
        // If leaving player was host, assign new host
        if (room->host_user_id == session->user_id && !room->player_ids.empty()) {
            for (uint32_t player_id : room->player_ids) {
                if (player_id != session->user_id) {
                    notif.new_host_user_id = player_id;
                    room->host_user_id = player_id;
                    break;
                }
            }
        }
        
        for (uint32_t player_id : room->player_ids) {
            if (player_id != session->user_id) {
                Session* player_session = nullptr;
                for (auto* s : g_session_mgr->get_all_sessions()) {
                    if (s->user_id == player_id) {
                        player_session = s;
                        break;
                    }
                }
                if (player_session) {
                    send_message(player_session->socket_fd, MessageType::S2C_PLAYER_LEFT_NOTIF,
                               &notif, sizeof(notif));
                }
            }
        }
        
        g_room_mgr->leave_room(session->current_room_id, session->user_id);
        session->current_room_id = 0;
    }
}

// Game loop function (would run in separate thread per room)
void run_game_loop(Room* room) {
    if (!room || !room->is_in_game) return;
    
    uint32_t session_id = room->room_id;
    g_db->save_game_session(session_id, 
                           room->game_mode == GameMode::ELIMINATION ? "Elimination" : "Scoring");
    
    for (size_t i = 0; i < room->questions.size(); i++) {
        room->current_question_index = i;
        QuestionNotification& q = room->questions[i];
        
        // Send question to all players
        for (uint32_t player_id : room->player_ids) {
            if (room->game_mode == GameMode::ELIMINATION && room->player_eliminated[player_id]) {
                continue; // Skip eliminated players
            }
            
            Session* player_session = nullptr;
            for (auto* s : g_session_mgr->get_all_sessions()) {
                if (s->user_id == player_id) {
                    player_session = s;
                    break;
                }
            }
            if (player_session) {
                send_message(player_session->socket_fd, MessageType::S2C_QUESTION_NOTIF,
                           &q, sizeof(q));
            }
        }
        
        // Wait for answers (simplified - would use proper timeout mechanism)
        std::this_thread::sleep_for(std::chrono::seconds(q.time_limit_sec + 2));
        
        // Process round results
        RoundResultNotification round_result;
        round_result.correct_option = g_db->get_correct_option(q.question_id);
        round_result.result_count = 0;
        
        for (uint32_t player_id : room->player_ids) {
            if (room->game_mode == GameMode::ELIMINATION && room->player_eliminated[player_id]) {
                continue;
            }
            
            PlayerRoundResult result;
            result.user_id = player_id;
            result.total_score = room->player_scores[player_id];
            result.correct_option = round_result.correct_option;
            result.was_eliminated = room->player_eliminated[player_id];
            result.answered_question = true; // Simplified
            
            round_result.results[round_result.result_count++] = result;
        }
        
        // Send round results to all players
        for (uint32_t player_id : room->player_ids) {
            Session* player_session = nullptr;
            for (auto* s : g_session_mgr->get_all_sessions()) {
                if (s->user_id == player_id) {
                    player_session = s;
                    break;
                }
            }
            if (player_session) {
                send_message(player_session->socket_fd, MessageType::S2C_ROUND_RESULT_NOTIF,
                           &round_result, sizeof(round_result));
            }
        }
        
        // Check for game end conditions
        if (room->game_mode == GameMode::ELIMINATION) {
            int active_players = 0;
            for (uint32_t player_id : room->player_ids) {
                if (!room->player_eliminated[player_id]) {
                    active_players++;
                }
            }
            if (active_players <= 1) {
                break; // Game over
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Brief pause between questions
    }
    
    // Game over - send final results
    GameOverNotification game_over;
    game_over.result_count = 0;
    
    // Sort players by score/rank
    std::vector<std::pair<uint32_t, int32_t>> player_scores_vec;
    for (uint32_t player_id : room->player_ids) {
        int32_t score = room->player_scores[player_id];
        if (room->game_mode == GameMode::ELIMINATION && room->player_eliminated[player_id]) {
            score = -1; // Eliminated players get lowest rank
        }
        player_scores_vec.push_back({player_id, score});
    }
    
    std::sort(player_scores_vec.begin(), player_scores_vec.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    uint32_t rank = 1;
    for (const auto& pair : player_scores_vec) {
        PlayerFinalResult result;
        result.user_id = pair.first;
        result.final_rank = rank++;
        result.final_score = pair.second;
        
        Session* player_session = nullptr;
        for (auto* s : g_session_mgr->get_all_sessions()) {
            if (s->user_id == pair.first) {
                player_session = s;
                strncpy(result.display_name, s->display_name.c_str(), MAX_DISPLAY_NAME_LEN - 1);
                break;
            }
        }
        
        game_over.results[game_over.result_count++] = result;
        
        // Save to database
        g_db->save_participant_result(session_id, pair.first, pair.second, result.final_rank);
    }
    
    // Send game over to all players
    for (uint32_t player_id : room->player_ids) {
        Session* player_session = nullptr;
        for (auto* s : g_session_mgr->get_all_sessions()) {
            if (s->user_id == player_id) {
                player_session = s;
                break;
            }
        }
        if (player_session) {
            send_message(player_session->socket_fd, MessageType::S2C_GAME_OVER_NOTIF,
                       &game_over, sizeof(game_over));
            player_session->current_room_id = 0;
        }
    }
    
    // Cleanup room
    g_room_mgr->delete_room(room->room_id);
}

// Client handler thread
void handle_client(int socket_fd) {
    char buffer[4096];
    
    while (true) {
        MessageHeader header;
        ssize_t bytes_received = recv(socket_fd, &header, sizeof(header), 0);
        
        if (bytes_received <= 0) {
            break;
        }
        
        if (header.body_len > 0 && header.body_len < sizeof(buffer)) {
            bytes_received = recv(socket_fd, buffer, header.body_len, 0);
            if (bytes_received <= 0) break;
        }
        
        switch (header.type) {
            case MessageType::C2S_REGISTER_REQ: {
                if (header.body_len >= sizeof(RegisterRequest)) {
                    RegisterRequest* req = reinterpret_cast<RegisterRequest*>(buffer);
                    handle_register(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_LOGIN_REQ: {
                if (header.body_len >= sizeof(LoginRequest)) {
                    LoginRequest* req = reinterpret_cast<LoginRequest*>(buffer);
                    handle_login(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_ACCOUNT_UPDATE_REQ: {
                if (header.body_len >= sizeof(AccountUpdateRequest)) {
                    AccountUpdateRequest* req = reinterpret_cast<AccountUpdateRequest*>(buffer);
                    handle_account_update(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_LIST_ROOMS_REQ: {
                handle_list_rooms(socket_fd);
                break;
            }
            case MessageType::C2S_CREATE_ROOM_REQ: {
                if (header.body_len >= sizeof(CreateRoomRequest)) {
                    CreateRoomRequest* req = reinterpret_cast<CreateRoomRequest*>(buffer);
                    handle_create_room(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_JOIN_ROOM_REQ: {
                if (header.body_len >= sizeof(JoinRoomRequest)) {
                    JoinRoomRequest* req = reinterpret_cast<JoinRoomRequest*>(buffer);
                    handle_join_room(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_READY_STATUS_REQ: {
                if (header.body_len >= sizeof(ReadyStatusRequest)) {
                    ReadyStatusRequest* req = reinterpret_cast<ReadyStatusRequest*>(buffer);
                    handle_ready_status(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_GET_STATS_REQ: {
                handle_get_stats(socket_fd);
                break;
            }
            case MessageType::C2S_SUBMIT_ANSWER_REQ: {
                if (header.body_len >= sizeof(SubmitAnswerRequest)) {
                    SubmitAnswerRequest* req = reinterpret_cast<SubmitAnswerRequest*>(buffer);
                    handle_submit_answer(socket_fd, *req);
                }
                break;
            }
            case MessageType::C2S_LEAVE_ROOM_REQ:
            case MessageType::C2S_LEAVE_MATCH_REQ: {
                handle_leave_room(socket_fd);
                break;
            }
            default:
                send_error(socket_fd, StatusCode::INVALID_REQUEST, "Unknown message type");
                break;
        }
    }
    
    // Cleanup
    Session* session = g_session_mgr->get_session(socket_fd);
    if (session && session->current_room_id != 0) {
        g_room_mgr->leave_room(session->current_room_id, session->user_id);
    }
    g_session_mgr->remove_session(socket_fd);
    close(socket_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    
    int port = std::stoi(argv[1]);
    
    // Initialize database
    try {
        g_db = new Database(DB_PATH);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize database: " << e.what() << std::endl;
        return 1;
    }
    
    // Initialize managers
    g_session_mgr = new SessionManager();
    g_room_mgr = new RoomManager();
    
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }
    
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    
    std::cout << "Server listening on port " << port << std::endl;
    
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            continue;
        }
        
        std::cout << "New client connected from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        std::thread client_thread(handle_client, client_fd);
        client_thread.detach();
    }
    
    close(server_fd);
    delete g_db;
    delete g_session_mgr;
    delete g_room_mgr;
    
    return 0;
}
