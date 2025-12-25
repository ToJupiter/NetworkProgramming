// test_client.cpp
// Interactive test client for the quiz game server
// Compile with: g++ test_client.cpp -o test_client -std=c++17

#include "../common/protocol.h"  // Make sure this path points to your protocol.h
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <errno.h>

class TestClient {
private:
    int sockfd;
    std::vector<uint8_t> recvBuffer;

    void sendMessage(MessageType type, const void* body = nullptr, uint32_t body_len = 0) {
        MessageHeader header;
        header.type = type;
        header.body_len = htonl(body_len);

        std::vector<uint8_t> packet;
        packet.insert(packet.end(), (uint8_t*)&header, (uint8_t*)&header + sizeof(header));
        if (body && body_len > 0) {
            packet.insert(packet.end(), (uint8_t*)body, (uint8_t*)body + body_len);
        }

        ssize_t sent = ::send(sockfd, packet.data(), packet.size(), 0);
        if (sent != static_cast<ssize_t>(packet.size())) {
            std::cerr << "Failed to send full message\n";
        }
    }

    // Return values:
    //  >0 : data processed
    //   0 : no data available (would block)
    //  <0 : connection closed or error
    int receiveMessage() {
        uint8_t temp[4096];
        ssize_t bytes = recv(sockfd, temp, sizeof(temp), MSG_DONTWAIT);
        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0; // no data available
            std::cerr << "[Recv error] " << strerror(errno) << "\n";
            return -1;
        }
        if (bytes == 0) {
            std::cout << "\n[Server disconnected]\n";
            return -1;
        }

        recvBuffer.insert(recvBuffer.end(), temp, temp + bytes);

        while (recvBuffer.size() >= sizeof(MessageHeader)) {
            MessageHeader header;
            std::memcpy(&header, recvBuffer.data(), sizeof(header));
            header.body_len = ntohl(header.body_len);

            if (recvBuffer.size() < sizeof(MessageHeader) + header.body_len) break;

            std::vector<uint8_t> body(
                recvBuffer.begin() + sizeof(MessageHeader),
                recvBuffer.begin() + sizeof(MessageHeader) + header.body_len
            );

            handleMessage(header.type, body.data(), header.body_len);

            recvBuffer.erase(recvBuffer.begin(),
                             recvBuffer.begin() + sizeof(MessageHeader) + header.body_len);
        }
        return 1;
    }

    void handleMessage(MessageType type, const void* data, uint32_t len) {
        switch (type) {
            case MessageType::S2C_REGISTER_RSP: {
                if (len >= sizeof(StatusResponse)) {
                    auto* rsp = (const StatusResponse*)data;
                    std::cout << "[Register] " << (rsp->code == StatusCode::SUCCESS ? "Success" : "Failed") << "\n";
                }
                break;
            }
            case MessageType::S2C_LOGIN_RSP: {
                if (len >= sizeof(LoginResponse)) {
                    auto* rsp = (const LoginResponse*)data;
                    if (rsp->code == StatusCode::SUCCESS) {
                        std::cout << "[Login Success] ID: " << rsp->user_id
                                  << " Name: " << rsp->display_name << "\n";
                    } else {
                        std::cout << "[Login Failed] Invalid credentials\n";
                    }
                }
                break;
            }
            case MessageType::S2C_LIST_ROOMS_RSP: {
                if (len >= sizeof(ListRoomsResponse)) {
                    auto* rsp = (const ListRoomsResponse*)data;
                    std::cout << "=== Room List (" << rsp->room_count << ") ===\n";
                    for (int i = 0; i < rsp->room_count; ++i) {
                        auto& room = rsp->rooms[i];
                        std::cout << "ID: " << room.room_id
                                  << " | Name: " << room.room_name
                                  << " | Players: " << (int)room.current_players << "/" << (int)room.max_players
                                  << " | Mode: " << (room.game_mode == GameMode::ELIMINATION ? "Elimination" : "Scoring")
                                  << " | " << (room.is_in_game ? "In Game" : "Waiting") << "\n";
                    }
                }
                break;
            }
            case MessageType::S2C_CREATE_ROOM_RSP:
            case MessageType::S2C_JOIN_ROOM_RSP: {
                StatusCode code = StatusCode::FAILURE_GENERIC;
                RoomInfo info{};
                if (len >= sizeof(CreateRoomResponse)) {
                    auto* rsp = (const CreateRoomResponse*)data;
                    code = rsp->code;
                    info = rsp->room_info;
                } else if (len >= sizeof(JoinRoomResponse)) {
                    auto* rsp = (const JoinRoomResponse*)data;
                    code = rsp->code;
                    info = rsp->room_info;
                }
                if (code == StatusCode::SUCCESS) {
                    std::cout << "[Room " << (type == MessageType::S2C_CREATE_ROOM_RSP ? "Created" : "Joined")
                              << "] ID: " << info.room_id << " | " << info.room_name << "\n";
                } else {
                    std::cout << "[Room Action Failed] Code: " << (int)code << "\n";
                }
                break;
            }
            case MessageType::S2C_PLAYER_JOINED_NOTIF: {
                if (len >= sizeof(PlayerInfo)) {
                    auto* p = (const PlayerInfo*)data;
                    std::cout << "[Player Joined] " << p->display_name << " (ID: " << p->user_id << ")\n";
                }
                break;
            }
            case MessageType::S2C_PLAYER_LEFT_NOTIF: {
                if (len >= sizeof(PlayerLeftNotification)) {
                    auto* n = (const PlayerLeftNotification*)data;
                    std::cout << "[Player Left] User ID: " << n->user_id
                              << " | New Host: " << n->new_host_user_id << "\n";
                }
                break;
            }
            case MessageType::S2C_READY_STATUS_NOTIF: {
                if (len >= sizeof(ReadyStatusNotification)) {
                    auto* n = (const ReadyStatusNotification*)data;
                    std::cout << "[Ready Status] " << n->user_id << " is now "
                              << (n->is_ready ? "READY" : "NOT READY") << "\n";
                }
                break;
            }
            case MessageType::S2C_GAME_START_NOTIF: {
                std::cout << "[GAME STARTING SOON...]\n";
                break;
            }
            case MessageType::S2C_QUESTION_NOTIF: {
                if (len >= sizeof(QuestionNotification)) {
                    auto* q = (const QuestionNotification*)data;
                    // capture the question id so the interactive loop can submit an answer
                    setCurrentQuestionId(q->question_id);
                    std::cout << "\n=== QUESTION " << q->question_id << " (Time: " << (int)q->time_limit_sec << "s) ===\n";
                    std::cout << q->content << "\n";
                    for (int i = 0; i < 4; ++i) {
                        std::cout << "  " << i << ") " << q->options[i] << "\n";
                    }
                    std::cout << "Your answer (0-3): ";
                    std::cout.flush();
                }
                break;
            }
            case MessageType::S2C_ROUND_RESULT_NOTIF: {
                if (len >= sizeof(RoundResultNotification)) {
                    auto* n = (const RoundResultNotification*)data;
                    std::cout << "\n--- ROUND RESULT ---\n";
                    std::cout << "Correct option: " << (int)n->correct_option << "\n";
                    for (int i = 0; i < n->result_count; ++i) {
                        auto& r = n->results[i];
                        std::cout << "Player " << r.user_id
                                  << " | Score change: " << r.score_change
                                  << " | Total: " << r.total_score
                                  << (r.was_eliminated ? " [ELIMINATED]" : "")
                                  << (r.answered_question ? "" : " [No answer]")
                                  << "\n";
                    }
                }
                break;
            }
            case MessageType::S2C_PLAYER_ELIMINATED_NOTIF: {
                if (len >= sizeof(PlayerEliminatedNotification)) {
                    auto* n = (const PlayerEliminatedNotification*)data;
                    std::cout << "[ELIMINATED] Player " << n->user_id << "\n";
                }
                break;
            }
            case MessageType::S2C_GAME_OVER_NOTIF: {
                if (len >= sizeof(GameOverNotification)) {
                    auto* n = (const GameOverNotification*)data;
                    std::cout << "\n=== GAME OVER ===\n";
                    for (int i = 0; i < n->result_count; ++i) {
                        auto& r = n->results[i];
                        std::cout << r.final_rank << ". " << r.display_name
                                  << " - Score: " << r.final_score << "\n";
                    }
                }
                break;
            }
            case MessageType::S2C_GET_STATS_RSP: {
                if (len >= sizeof(UserStatsResponse)) {
                    auto* s = (const UserStatsResponse*)data;
                    std::cout << "=== Your Stats ===\n";
                    std::cout << "Matches: " << s->total_matches << " | Wins: " << s->wins << "\n";
                    std::cout << "Correct: " << s->total_correct_answers
                              << " | Incorrect: " << s->total_incorrect_answers << "\n";
                    std::cout << "Average Score: " << s->average_score << "\n";
                    std::cout << "Ranked Points (Elo): " << s->ranked_points << "\n";
                }
                break;
            }
            case MessageType::S2C_GAME_PAUSED_NOTIF:
                std::cout << "[GAME PAUSED]\n";
                break;
            case MessageType::S2C_GAME_RESUMED_NOTIF:
                std::cout << "[GAME RESUMED]\n";
                break;
            case MessageType::S2C_GAME_TERMINATED_NOTIF: {
                if (len >= sizeof(GameTerminatedNotification)) {
                    auto* n = (const GameTerminatedNotification*)data;
                    std::cout << "[GAME TERMINATED] Reason: " << (int)n->reason << "\n";
                }
                break;
            }
            case MessageType::S2C_ERROR_RSP: {
                if (len >= sizeof(ErrorResponse)) {
                    auto* e = (const ErrorResponse*)data;
                    std::cout << "[SERVER ERROR] " << e->message << "\n";
                }
                break;
            }
            default:
                std::cout << "[Unknown message type: " << (int)type << "]\n";
                break;
        }
    }

public:
    bool connectToServer(const std::string& ip, int port) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "Socket creation failed\n";
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

        if (::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Connection failed\n";
            return false;
        }

        int opt = 1;
        setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

        std::cout << "Connected to server " << ip << ":" << port << "\n";
        return true;
    }

    void runInteractive() {
        std::string line;
        // use the member `current_question_id` (set by incoming question notifications)

        while (true) {
            std::cout << "> ";
            std::cout.flush();

            if (!std::getline(std::cin, line)) break;

            if (line.empty()) {
                int rv = receiveMessage();
                if (rv < 0) break;
                continue;
            }

            // Special: if waiting for answer and user types 0-3, send answer
            if (this->current_question_id != 0 && line.size() == 1 && line[0] >= '0' && line[0] <= '3') {
                uint8_t opt = line[0] - '0';
                SubmitAnswerRequest req{};
                req.question_id = this->current_question_id;
                req.selected_option = opt;
                req.response_time_ms = 5000; // simulated
                sendMessage(MessageType::C2S_SUBMIT_ANSWER_REQ, &req, sizeof(req));
                this->current_question_id = 0;
                continue;
            }

            if (line == "register") {
                RegisterRequest req{};
                std::cout << "Email: "; std::getline(std::cin, line); strncpy(req.email, line.c_str(), MAX_EMAIL_LEN-1);
                std::cout << "Display Name: "; std::getline(std::cin, line); strncpy(req.display_name, line.c_str(), MAX_DISPLAY_NAME_LEN-1);
                std::cout << "Password: "; std::getline(std::cin, line); strncpy(req.password, line.c_str(), MAX_PASSWORD_LEN-1);
                sendMessage(MessageType::C2S_REGISTER_REQ, &req, sizeof(req));
            }
            else if (line == "login") {
                LoginRequest req{};
                std::cout << "Email: "; std::getline(std::cin, line); strncpy(req.email, line.c_str(), MAX_EMAIL_LEN-1);
                std::cout << "Password: "; std::getline(std::cin, line); strncpy(req.password, line.c_str(), MAX_PASSWORD_LEN-1);
                sendMessage(MessageType::C2S_LOGIN_REQ, &req, sizeof(req));
            }
            else if (line == "list") {
                sendMessage(MessageType::C2S_LIST_ROOMS_REQ);
            }
            else if (line == "create") {
                CreateRoomRequest req{};
                std::cout << "Room Name: "; std::getline(std::cin, line); strncpy(req.room_name, line.c_str(), MAX_ROOM_NAME_LEN-1);
                std::cout << "Mode (0=Elimination, 1=Scoring): "; 
                std::string gmode;
                std::cin >> gmode;
                req.game_mode = (gmode == "1") ? GameMode::SCORING : GameMode::ELIMINATION;
                std::cin.ignore();
                std::cout << "Number of questions: "; std::cin >> req.num_questions; std::cin.ignore();
                sendMessage(MessageType::C2S_CREATE_ROOM_REQ, &req, sizeof(req));
            }
            else if (line.substr(0, 4) == "join") {
                uint32_t room_id = std::stoul(line.substr(5));
                JoinRoomRequest req{room_id};
                sendMessage(MessageType::C2S_JOIN_ROOM_REQ, &req, sizeof(req));
            }
            else if (line == "leave") {
                sendMessage(MessageType::C2S_LEAVE_ROOM_REQ);
            }
            else if (line == "ready") {
                ReadyStatusRequest req{true};
                sendMessage(MessageType::C2S_READY_STATUS_REQ, &req, sizeof(req));
            }
            else if (line == "unready") {
                ReadyStatusRequest req{false};
                sendMessage(MessageType::C2S_READY_STATUS_REQ, &req, sizeof(req));
            }
            else if (line == "stats") {
                sendMessage(MessageType::C2S_GET_STATS_REQ);
            }
            else if (line == "pause") {
                sendMessage(MessageType::C2S_PAUSE_GAME_REQ);
            }
            else if (line == "resume") {
                sendMessage(MessageType::C2S_RESUME_GAME_REQ);
            }
            else if (line == "quit") {
                break;
            }
            else if (line == "help") {
                std::cout << "Commands:\n"
                          << "  register     - register new account\n"
                          << "  login        - login\n"
                          << "  list         - list rooms\n"
                          << "  create       - create room\n"
                          << "  join <id>    - join room\n"
                          << "  leave        - leave room\n"
                          << "  ready        - mark ready\n"
                          << "  unready      - mark not ready\n"
                          << "  stats        - view your stats\n"
                          << "  pause        - pause game (host only)\n"
                          << "  resume       - resume game (host only)\n"
                          << "  quit         - exit\n"
                          << "During question: just type 0-3 to answer\n";
            }
            else {
                std::cout << "Unknown command. Type 'help'\n";
            }

            // Flush any incoming messages that are currently available
            while (receiveMessage() > 0) {}
        }

        close(sockfd);
    }

    void setCurrentQuestionId(uint32_t qid) {
        current_question_id = qid;
        // Auto-reset after a while if not answered
    }

    uint32_t current_question_id = 0;

    // Override question notification to capture ID
    void handleQuestion(const QuestionNotification& q) {
        current_question_id = q.question_id;
        // ... existing print code ...
    }
};

int main() {
    TestClient client;
    if (!client.connectToServer("127.0.0.1", 10002)) {
        return 1;
    }

    std::cout << "Type 'help' for commands.\n";
    client.runInteractive();
    return 0;
}
