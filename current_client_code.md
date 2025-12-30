# Source Code Collection

## client/CMakeLists.txt
``` 
cmake_minimum_required(VERSION 3.16)
project(QuizClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Prefer Qt6 on Linux, fallback to Qt5 if not available
find_package(Qt6 COMPONENTS Core Widgets Network QUIET)
if (Qt6_FOUND)
    set(QT_PACKAGE Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)
    set(QT_PACKAGE Qt5)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)

# Source files (Phase 1-4)
set(SOURCES
    src/main.cpp
    src/network/NetworkManager.cpp
    src/network/ProtocolHelper.cpp
    src/network/PosixSocketClient.cpp
    src/ui/LoginWindow.cpp
    src/ui/MainMenuWindow.cpp
    src/ui/LobbyWindow.cpp
    src/ui/CreateRoomDialog.cpp
    src/ui/RoomWindow.cpp
    src/ui/GameWindow.cpp
    src/ui/ReplayWindow.cpp
    src/ui/GameHistoryWindow.cpp
    src/ui/StatsWindow.cpp
    src/models/SessionState.cpp
    src/models/GameState.cpp
)

set(HEADERS
    src/network/NetworkManager.h
    src/network/ProtocolHelper.h
    src/network/PosixSocketClient.h
    src/ui/LoginWindow.h
    src/ui/MainMenuWindow.h
    src/ui/LobbyWindow.h
    src/ui/CreateRoomDialog.h
    src/ui/RoomWindow.h
    src/ui/GameWindow.h
    src/ui/ReplayWindow.h
    src/ui/GameHistoryWindow.h
    src/ui/StatsWindow.h
    src/models/SessionState.h
    src/models/GameState.h
    ../common/protocol.h
)

set(UI_FILES
    src/ui/LoginWindow.ui
    src/ui/MainMenuWindow.ui
    src/ui/LobbyWindow.ui
    src/ui/CreateRoomDialog.ui
    src/ui/RoomWindow.ui
    src/ui/GameWindow.ui
    src/ui/ReplayWindow.ui
    src/ui/GameHistoryWindow.ui
    src/ui/StatsWindow.ui
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(${PROJECT_NAME}
    ${QT_PACKAGE}::Core
    ${QT_PACKAGE}::Widgets
)
```


## These code below are all inside the folder client/src/

##  main.cpp 

```cpp
#include <QApplication>
#include "ui/LoginWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    LoginWindow w;
    w.show();
    return app.exec();
}

```

---

##  models/GameState.h 

```cpp
#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <cstdint>
#include "protocol.h"

// Local client-side player state (distinct from protocol::PlayerInfo)
struct ClientPlayer {
    uint32_t user_id;
    QString display_name;
    bool is_ready;
    uint32_t score;
    bool is_eliminated;
    
    ClientPlayer() : user_id(0), is_ready(false), score(0), is_eliminated(false) {}
};

struct QuestionData {
    uint32_t questionId;
    QString content;
    QString options[4];
    uint8_t correctOption;  // Known after round ends
    uint32_t timeLimitSec;
    
    QuestionData() : questionId(0), correctOption(255), timeLimitSec(0) {}
};

class GameState : public QObject {
    Q_OBJECT
    
public:
    GameState(QObject* parent = nullptr);
    
    // Room info
    void setRoomInfo(uint32_t roomId, const QString& roomName, GameMode mode, uint8_t numQuestions);
    uint32_t getRoomId() const { return m_roomId; }
    QString getRoomName() const { return m_roomName; }
    GameMode getGameMode() const { return m_gameMode; }
    uint8_t getNumQuestions() const { return m_numQuestions; }
    
    // Players
    void clearPlayers();
    void addPlayer(const ClientPlayer& player);
    void removePlayer(uint32_t userId);
    void updatePlayerReady(uint32_t userId, bool ready);
    void updatePlayerScore(uint32_t userId, uint32_t score, bool eliminated);
    QVector<ClientPlayer> getPlayers() const { return m_players; }
    ClientPlayer* findPlayer(uint32_t userId);
    
    // Current question
    void setCurrentQuestion(const QuestionData& question);
    QuestionData getCurrentQuestion() const { return m_currentQuestion; }
    void setCurrentQuestionIndex(uint8_t index) { m_currentQuestionIndex = index; }
    uint8_t getCurrentQuestionIndex() const { return m_currentQuestionIndex; }
    
    // My answer
    void setMyAnswer(uint8_t option) { m_mySelectedOption = option; }
    uint8_t getMyAnswer() const { return m_mySelectedOption; }
    
    // My score
    void setMyScore(uint32_t score) { m_myScore = score; }
    uint32_t getMyScore() const { return m_myScore; }
    
    // Reset
    void clear();
    
signals:
    void playerListChanged();
    void questionChanged();
    void scoresUpdated();
    
private:
    uint32_t m_roomId;
    QString m_roomName;
    GameMode m_gameMode;
    uint8_t m_numQuestions;
    
    QVector<ClientPlayer> m_players;
    QuestionData m_currentQuestion;
    uint8_t m_currentQuestionIndex;
    
    uint8_t m_mySelectedOption;
    uint32_t m_myScore;
};

```

---

##  models/SessionState.h 

```cpp
#pragma once

#include <QString>
#include <cstdint>

class SessionState {
public:
    SessionState();
    // Global singleton access
    static SessionState& instance();
    
    // Authentication state
    bool isAuthenticated() const { return m_isAuthenticated; }
    void setAuthenticated(bool auth) { m_isAuthenticated = auth; }
    
    // User info
    uint32_t getUserId() const { return m_userId; }
    void setUserId(uint32_t id) { m_userId = id; }
    
    QString getDisplayName() const { return m_displayName; }
    void setDisplayName(const QString& name) { m_displayName = name; }
    
    QString getEmail() const { return m_email; }
    void setEmail(const QString& email) { m_email = email; }
    
    // Room state
    uint32_t getCurrentRoomId() const { return m_currentRoomId; }
    void setCurrentRoomId(uint32_t id) { m_currentRoomId = id; }
    
    bool isInRoom() const { return m_currentRoomId != 0; }
    
    // Reset
    void clear();
    
private:
    bool m_isAuthenticated;
    uint32_t m_userId;
    QString m_displayName;
    QString m_email;
    uint32_t m_currentRoomId;
};

```

---

##  models/GameState.cpp 

```cpp
#include "GameState.h"

GameState::GameState(QObject* parent)
    : QObject(parent)
    , m_roomId(0)
    , m_gameMode(GameMode::ELIMINATION)
    , m_numQuestions(0)
    , m_currentQuestionIndex(0)
    , m_mySelectedOption(0)
    , m_myScore(0)
{
}

void GameState::setRoomInfo(uint32_t roomId, const QString& roomName, GameMode mode, uint8_t numQuestions) {
    m_roomId = roomId;
    m_roomName = roomName;
    m_gameMode = mode;
    m_numQuestions = numQuestions;
}

void GameState::clearPlayers() {
    m_players.clear();
    emit playerListChanged();
}

void GameState::addPlayer(const ClientPlayer& player) {
    m_players.append(player);
    emit playerListChanged();
}

void GameState::removePlayer(uint32_t userId) {
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i].user_id == userId) {
            m_players.removeAt(i);
            emit playerListChanged();
            break;
        }
    }
}

void GameState::updatePlayerReady(uint32_t userId, bool ready) {
    ClientPlayer* player = findPlayer(userId);
    if (player) {
        player->is_ready = ready;
        emit playerListChanged();
    }
}

void GameState::updatePlayerScore(uint32_t userId, uint32_t score, bool eliminated) {
    ClientPlayer* player = findPlayer(userId);
    if (player) {
        player->score = score;
        player->is_eliminated = eliminated;
        emit scoresUpdated();
    }
}

ClientPlayer* GameState::findPlayer(uint32_t userId) {
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i].user_id == userId) {
            return &m_players[i];
        }
    }
    return nullptr;
}

void GameState::setCurrentQuestion(const QuestionData& question) {
    m_currentQuestion = question;
    m_mySelectedOption = 0;
    emit questionChanged();
}

void GameState::clear() {
    m_roomId = 0;
    m_roomName.clear();
    m_numQuestions = 0;
    m_currentQuestionIndex = 0;
    m_mySelectedOption = 0;
    m_myScore = 0;
    clearPlayers();
}

```

---

##  models/SessionState.cpp 

```cpp
#include "SessionState.h"

SessionState::SessionState()
    : m_isAuthenticated(false)
    , m_userId(0)
    , m_currentRoomId(0)
{
}

SessionState& SessionState::instance() {
    static SessionState inst;
    return inst;
}

void SessionState::clear() {
    m_isAuthenticated = false;
    m_userId = 0;
    m_displayName.clear();
    m_email.clear();
    m_currentRoomId = 0;
}

```

---

##  network/NetworkManager.cpp 

```cpp
#include "NetworkManager.h"
#include "ProtocolHelper.h"
#include <QDebug>
#include <poll.h>

NetworkManager& NetworkManager::instance() {
    static NetworkManager inst;
    return inst;
}

NetworkManager::NetworkManager()
    : QObject(nullptr)
    , m_socket(std::make_unique<PosixSocketClient>())
    , m_ioTimer(new QTimer(this))
{
    connect(m_ioTimer, &QTimer::timeout, this, &NetworkManager::onIOReady);
    m_ioTimer->setInterval(50);
}

NetworkManager::~NetworkManager() {
    if (m_socket && m_socket->isConnected()) {
        m_socket->disconnect();
    }
    if (m_ioTimer) {
        m_ioTimer->stop();
    }
}

bool NetworkManager::connectToServer(const QString& host, quint16 port) {
    if (m_socket->isConnected()) {
        return true;
    }
    
    if (m_socket->connect(host.toStdString(), port)) {
        m_ioTimer->start();
        onConnected();
        return true;
    }
    return false;
}

void NetworkManager::disconnectFromServer() {
    if (m_socket && m_socket->isConnected()) {
        m_socket->disconnect();
        m_ioTimer->stop();
    }
}

bool NetworkManager::isConnected() const {
    return m_socket && m_socket->isConnected();
}

void NetworkManager::sendMessage(MessageType type, const QByteArray& body) {
    if (!isConnected()) {
        qWarning() << "Cannot send message: not connected";
        return;
    }
    
    QByteArray message = ProtocolHelper::createMessage(type, body);
    if (!m_socket->send(reinterpret_cast<const uint8_t*>(message.constData()), message.size())) {
        qWarning() << "Failed to send message";
    }
}

// === Message Senders ===

void NetworkManager::sendRegister(const QString& email, const QString& displayName, const QString& password) {
    RegisterRequest req;
    std::memset(&req, 0, sizeof(req));
    ProtocolHelper::copyToFixedArray(req.email, MAX_EMAIL_LEN, email);
    ProtocolHelper::copyToFixedArray(req.display_name, MAX_DISPLAY_NAME_LEN, displayName);
    ProtocolHelper::copyToFixedArray(req.password, MAX_PASSWORD_LEN, password);
    
    sendMessage(MessageType::C2S_REGISTER_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendLogin(const QString& email, const QString& password) {
    LoginRequest req;
    std::memset(&req, 0, sizeof(req));
    ProtocolHelper::copyToFixedArray(req.email, MAX_EMAIL_LEN, email);
    ProtocolHelper::copyToFixedArray(req.password, MAX_PASSWORD_LEN, password);
    
    sendMessage(MessageType::C2S_LOGIN_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendCreateRoom(const QString& roomName, GameMode mode, uint8_t numQuestions) {
    CreateRoomRequest req;
    std::memset(&req, 0, sizeof(req));
    ProtocolHelper::copyToFixedArray(req.room_name, MAX_ROOM_NAME_LEN, roomName);
    req.game_mode = mode;
    req.num_questions = numQuestions;
    
    sendMessage(MessageType::C2S_CREATE_ROOM_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendListRooms() {
    sendMessage(MessageType::C2S_LIST_ROOMS_REQ, QByteArray());
}

void NetworkManager::sendJoinRoom(uint32_t roomId) {
    qDebug() << "[NetworkManager] Sending JOIN_ROOM_REQ for roomId=" << roomId;
    JoinRoomRequest req;
    req.room_id = roomId;
    
    sendMessage(MessageType::C2S_JOIN_ROOM_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendLeaveRoom() {
    sendMessage(MessageType::C2S_LEAVE_ROOM_REQ, QByteArray());
}

void NetworkManager::sendReadyStatus(bool ready) {
    ReadyStatusRequest req;
    req.is_ready = ready ? 1 : 0;
    
    sendMessage(MessageType::C2S_READY_STATUS_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendStartGame() {
    qDebug() << "[NetworkManager] Sending START_GAME_REQ";
    sendMessage(MessageType::C2S_START_GAME_REQ, QByteArray());
}

void NetworkManager::sendSubmitAnswer(uint32_t questionId, uint8_t selectedOption, uint32_t responseTimeMs) {
    SubmitAnswerRequest req;
    req.question_id = questionId;
    req.selected_option = selectedOption;
    req.response_time_ms = responseTimeMs;
    
    sendMessage(MessageType::C2S_SUBMIT_ANSWER_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendReturnToRoom() {
    sendMessage(MessageType::C2S_RETURN_TO_ROOM_REQ, QByteArray());
}

void NetworkManager::sendGetStats() {
    sendMessage(MessageType::C2S_GET_STATS_REQ, QByteArray());
}

void NetworkManager::sendPauseGame() {
    sendMessage(MessageType::C2S_PAUSE_GAME_REQ, QByteArray());
}

void NetworkManager::sendResumeGame() {
    sendMessage(MessageType::C2S_RESUME_GAME_REQ, QByteArray());
}

void NetworkManager::sendGetReplay(uint32_t sessionId) {
    GetReplayRequest req;
    req.session_id = sessionId;
    
    sendMessage(MessageType::C2S_GET_REPLAY_REQ, ProtocolHelper::packStruct(req));
}

void NetworkManager::sendGetGameHistory() {
    sendMessage(MessageType::C2S_GET_GAME_HISTORY_REQ, QByteArray());
}

// === Socket Event Handlers ===

void NetworkManager::onConnected() {
    qDebug() << "Connected to server";
    emit connected();
}

void NetworkManager::onDisconnected() {
    qDebug() << "Disconnected from server";
    m_recvBuffer.clear();
    m_ioTimer->stop();
    emit disconnected();
}

void NetworkManager::onReadyRead() {
    uint8_t buffer[4096];
    int n = m_socket->recv(buffer, sizeof(buffer));
    if (n > 0) {
        m_recvBuffer.append(reinterpret_cast<char*>(buffer), n);
        processBuffer();
    } else if (n == -1) {
        onDisconnected();
    }
}

void NetworkManager::onSocketError(const QString& error) {
    qWarning() << "Socket error:" << error;
    emit connectionError(error);
}

void NetworkManager::onIOReady() {
    if (!isConnected()) {
        onDisconnected();
        return;
    }
    
    struct pollfd pfd;
    pfd.fd = m_socket->getFd();
    pfd.events = POLLIN;
    
    int ret = poll(&pfd, 1, 0);
    if (ret > 0 && (pfd.revents & POLLIN)) {
        onReadyRead();
    }
}

void NetworkManager::processBuffer() {
    while (m_recvBuffer.size() >= static_cast<int>(sizeof(MessageHeader))) {
        // Parse header
        MessageHeader header;
        std::memcpy(&header, m_recvBuffer.constData(), sizeof(MessageHeader));
        
        uint32_t bodyLen = ProtocolHelper::ntohl(header.body_len);
        uint32_t totalLen = sizeof(MessageHeader) + bodyLen;
        
        // Wait for complete message
        if (m_recvBuffer.size() < static_cast<int>(totalLen)) {
            break;
        }
        
        // Extract body
        QByteArray body = m_recvBuffer.mid(sizeof(MessageHeader), bodyLen);
        
        // Remove processed message
        m_recvBuffer.remove(0, totalLen);
        
        // Handle message
        handleMessage(header.type, body);
    }
}

void NetworkManager::handleMessage(MessageType type, const QByteArray& body) {
    switch (type) {
        case MessageType::S2C_REGISTER_RSP: {
            auto resp = ProtocolHelper::unpackStruct<StatusResponse>(body);
            emit registerResponse(resp.code);
            break;
        }
        
        case MessageType::S2C_LOGIN_RSP: {
            auto resp = ProtocolHelper::unpackStruct<LoginResponse>(body);
            QString displayName = ProtocolHelper::fromFixedArray(resp.display_name, MAX_DISPLAY_NAME_LEN);
            emit loginResponse(resp.code, resp.user_id, displayName);
            break;
        }
        
        case MessageType::S2C_CREATE_ROOM_RSP: {
            auto resp = ProtocolHelper::unpackStruct<CreateRoomResponse>(body);
            emit createRoomResponse(resp.code, resp.room_info);
            break;
        }
        
        case MessageType::S2C_LIST_ROOMS_RSP: {
            auto resp = ProtocolHelper::unpackStruct<ListRoomsResponse>(body);
            QVector<RoomInfo> rooms;
            for (int i = 0; i < resp.room_count; ++i) {
                rooms.append(resp.rooms[i]);
            }
            emit listRoomsResponse(resp.room_count, rooms);
            break;
        }
        
        case MessageType::S2C_JOIN_ROOM_RSP: {
            auto resp = ProtocolHelper::unpackStruct<JoinRoomResponse>(body);
            qDebug() << "[NetworkManager] Received JOIN_ROOM_RSP: code=" << (int)resp.code 
                     << "roomId=" << resp.room_info.room_id 
                     << "playerCount=" << (int)resp.player_count;
            QVector<PlayerInfo> players;
            for (int i = 0; i < resp.player_count; ++i) {
                players.append(resp.players[i]);
            }
            emit joinRoomResponse(resp.code, resp.room_info, resp.player_count, players, resp.host_user_id);
            break;
        }
        
        case MessageType::S2C_PLAYER_JOINED_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<PlayerInfo>(body);
            emit playerJoinedNotif(notif);
            break;
        }
        
        case MessageType::S2C_PLAYER_LEFT_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<PlayerLeftNotification>(body);
            emit playerLeftNotif(notif.user_id);
            break;
        }
        
        case MessageType::S2C_READY_STATUS_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<ReadyStatusNotification>(body);
            emit readyStatusNotif(notif.user_id, notif.is_ready != 0);
            break;
        }
        
        case MessageType::S2C_GAME_START_NOTIF: {
            emit gameStartNotif();
            break;
        }
        
        case MessageType::S2C_QUESTION_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<QuestionNotification>(body);
            QString content = ProtocolHelper::fromFixedArray(notif.content, MAX_QUESTION_CONTENT_LEN);
            QStringList options;
            for (int i = 0; i < 4; ++i) {
                options << ProtocolHelper::fromFixedArray(notif.options[i], MAX_OPTION_CONTENT_LEN);
            }
            emit questionNotif(notif.question_id, content, options, notif.time_limit_sec);
            break;
        }
        
        case MessageType::S2C_ROUND_RESULT_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<RoundResultNotification>(body);
            QVector<PlayerRoundResult> results;
            for (int i = 0; i < notif.result_count; ++i) {
                results.append(notif.results[i]);
            }
            emit roundResultNotif(notif.correct_option, notif.result_count, results);
            break;
        }
        
        case MessageType::S2C_PLAYER_ELIMINATED_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<PlayerEliminatedNotification>(body);
            emit playerEliminatedNotif(notif.user_id);
            break;
        }
        
        case MessageType::S2C_GAME_OVER_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<GameOverNotification>(body);
            QVector<PlayerFinalResult> rankings;
            for (int i = 0; i < notif.result_count; ++i) {
                rankings.append(notif.results[i]);
            }
            emit gameOverNotif(notif.result_count, rankings, (uint8_t)notif.end_reason, notif.winner_count);
            break;
        }
        
        case MessageType::S2C_GAME_PAUSED_NOTIF: {
            emit gamePausedNotif();
            break;
        }
        
        case MessageType::S2C_GAME_RESUMED_NOTIF: {
            emit gameResumedNotif();
            break;
        }
        
        case MessageType::S2C_GAME_TERMINATED_NOTIF: {
            auto notif = ProtocolHelper::unpackStruct<GameTerminatedNotification>(body);
            emit gameTerminatedNotif(notif.reason);
            break;
        }
        
        case MessageType::S2C_RETURN_TO_ROOM_RSP: {
            auto resp = ProtocolHelper::unpackStruct<StatusResponse>(body);
            emit returnToRoomResponse(resp.code);
            break;
        }
        
        case MessageType::S2C_GET_STATS_RSP: {
            auto resp = ProtocolHelper::unpackStruct<UserStatsResponse>(body);
            emit statsResponse(resp);
            break;
        }
        
        case MessageType::S2C_GET_REPLAY_RSP: {
            auto resp = ProtocolHelper::unpackStruct<ReplayDataResponse>(body);
            QVector<ReplayEvent> events;
            for (uint32_t i = 0; i < resp.event_count && i < 10000; ++i) {
                events.append(resp.events[i]);
            }
            emit replayDataResponse(resp.status, resp.session_id, resp.game_mode, events);
            break;
        }
        
        case MessageType::S2C_GET_GAME_HISTORY_RSP: {
            auto resp = ProtocolHelper::unpackStruct<GameHistoryResponse>(body);
            QVector<GameHistoryEntry> entries;
            for (uint32_t i = 0; i < resp.entry_count && i < 100; ++i) {
                entries.append(resp.entries[i]);
            }
            emit gameHistoryResponse(resp.status, entries);
            break;
        }
        
        case MessageType::S2C_ERROR_RSP: {
            auto resp = ProtocolHelper::unpackStruct<ErrorResponse>(body);
            QString msg = ProtocolHelper::fromFixedArray(resp.message, MAX_ERROR_MSG_LEN);
            emit errorResponse(resp.code, msg);
            break;
        }
        
        default:
            qWarning() << "Unknown message type:" << static_cast<uint16_t>(type);
            break;
    }
}

void NetworkManager::pumpEvents() {
    if (isConnected()) {
        onIOReady();
    }
}

```

---

##  network/PosixSocketClient.cpp 

```cpp
#include "PosixSocketClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <poll.h>

PosixSocketClient::PosixSocketClient()
    : m_socketFd(-1) {
}

PosixSocketClient::~PosixSocketClient() {
    disconnect();
}

bool PosixSocketClient::connect(const std::string& host, uint16_t port) {
    if (m_socketFd != -1) {
        return true;
    }

    m_socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socketFd == -1) {
        perror("socket");
        return false;
    }

    struct addrinfo hints, *result, *p;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        perror("getaddrinfo");
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    for (p = result; p != nullptr; p = p->ai_next) {
        if (::connect(m_socketFd, p->ai_addr, p->ai_addrlen) == 0) {
            freeaddrinfo(result);
            int flags = fcntl(m_socketFd, F_GETFL, 0);
            fcntl(m_socketFd, F_SETFL, flags | O_NONBLOCK);
            return true;
        }
    }

    freeaddrinfo(result);
    close(m_socketFd);
    m_socketFd = -1;
    perror("connect");
    return false;
}

void PosixSocketClient::disconnect() {
    if (m_socketFd != -1) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

bool PosixSocketClient::isConnected() const {
    if (m_socketFd == -1) {
        return false;
    }

    struct pollfd pfd;
    pfd.fd = m_socketFd;
    pfd.events = POLLOUT;

    int ret = poll(&pfd, 1, 0);
    if (ret <= 0) {
        return false;
    }

    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(m_socketFd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
        return false;
    }

    return error == 0;
}

bool PosixSocketClient::send(const uint8_t* data, size_t len) {
    if (m_socketFd == -1) {
        return false;
    }

    size_t sent = 0;
    while (sent < len) {
        ssize_t n = ::send(m_socketFd, data + sent, len - sent, 0);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                struct pollfd pfd;
                pfd.fd = m_socketFd;
                pfd.events = POLLOUT;
                if (poll(&pfd, 1, 5000) <= 0) {
                    return false;
                }
                continue;
            }
            perror("send");
            return false;
        }
        sent += n;
    }

    return true;
}

int PosixSocketClient::recv(uint8_t* buffer, size_t maxLen) {
    if (m_socketFd == -1) {
        return -1;
    }

    ssize_t n = ::recv(m_socketFd, buffer, maxLen, 0);
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        perror("recv");
        return -1;
    }

    return n;
}

```

---

##  network/ProtocolHelper.h 

```cpp
#pragma once

#include <QByteArray>
#include <QString>
#include <cstring>
#include "protocol.h"

class ProtocolHelper {
public:
    // Pack any struct into QByteArray
    template<typename T>
    static QByteArray packStruct(const T& data) {
        return QByteArray(reinterpret_cast<const char*>(&data), sizeof(T));
    }
    
    // Unpack QByteArray into struct
    template<typename T>
    static T unpackStruct(const QByteArray& data) {
        T result;
        std::memset(&result, 0, sizeof(T));
        if (data.size() >= static_cast<int>(sizeof(T))) {
            std::memcpy(&result, data.constData(), sizeof(T));
        }
        return result;
    }
    
    // Create message with header
    static QByteArray createMessage(MessageType type, const QByteArray& body);
    
    // Convert QString to fixed-size char array (for protocol structs)
    static void copyToFixedArray(char* dest, int maxLen, const QString& src);
    
    // Convert fixed-size char array to QString
    static QString fromFixedArray(const char* src, int maxLen);
    
    // Byte order conversion (if needed for cross-platform)
    static uint32_t htonl(uint32_t hostlong);
    static uint32_t ntohl(uint32_t netlong);
    static uint16_t htons(uint16_t hostshort);
    static uint16_t ntohs(uint16_t netshort);
};

```

---

##  network/NetworkManager.h 

```cpp
#pragma once

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <memory>
#include "protocol.h"
#include "PosixSocketClient.h"

class NetworkManager : public QObject {
    Q_OBJECT
    
public:
    static NetworkManager& instance();
    
    // Connection management
    bool connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Message sending
    void sendRegister(const QString& email, const QString& displayName, const QString& password);
    void sendLogin(const QString& email, const QString& password);
    void sendCreateRoom(const QString& roomName, GameMode mode, uint8_t numQuestions);
    void sendListRooms();
    void sendJoinRoom(uint32_t roomId);
    void sendLeaveRoom();
    void sendReadyStatus(bool ready);
    void sendStartGame();
    void sendSubmitAnswer(uint32_t questionId, uint8_t selectedOption, uint32_t responseTimeMs);
    void sendReturnToRoom();
    void sendGetStats();
    void sendPauseGame();
    void sendResumeGame();
    void sendGetReplay(uint32_t sessionId);
    void sendGetGameHistory();
    
signals:
    // Connection signals
    void connected();
    void disconnected();
    void connectionError(const QString& error);
    
    // Response signals
    void registerResponse(StatusCode code);
    void loginResponse(StatusCode code, uint32_t userId, const QString& displayName);
    void createRoomResponse(StatusCode code, const RoomInfo& roomInfo);
    void listRoomsResponse(uint16_t roomCount, const QVector<RoomInfo>& rooms);
    void joinRoomResponse(StatusCode code, const RoomInfo& roomInfo, 
                         uint8_t playerCount, const QVector<PlayerInfo>& players, uint32_t hostUserId);
    
    // Notifications
    void playerJoinedNotif(const PlayerInfo& player);
    void playerLeftNotif(uint32_t userId);
    void readyStatusNotif(uint32_t userId, bool ready);
    void gameStartNotif();
    void questionNotif(uint32_t questionId, const QString& content, 
                      const QStringList& options, uint32_t timeLimitSec);
    void roundResultNotif(uint8_t correctOption, uint8_t playerCount, const QVector<PlayerRoundResult>& results);
    void playerEliminatedNotif(uint32_t userId);
    void gameOverNotif(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings, 
                      uint8_t gameEndReason, uint8_t winnerCount);
    void gamePausedNotif();
    void gameResumedNotif();
    void gameTerminatedNotif(TerminationReason reason);
    
    void returnToRoomResponse(StatusCode code);
    void statsResponse(const UserStatsResponse& stats);
    void replayDataResponse(StatusCode status, uint32_t sessionId, GameMode mode, const QVector<ReplayEvent>& events);
    void gameHistoryResponse(StatusCode status, const QVector<GameHistoryEntry>& entries);
    void errorResponse(StatusCode code, const QString& message);
    
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(const QString& error);
    void onIOReady();
    
private:
    NetworkManager();
    ~NetworkManager();
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    void sendMessage(MessageType type, const QByteArray& body);
    void processBuffer();
    void handleMessage(MessageType type, const QByteArray& body);
    void pumpEvents();

    std::unique_ptr<PosixSocketClient> m_socket;
    QByteArray m_recvBuffer;
    QTimer* m_ioTimer;
};

```

---

##  network/ProtocolHelper.cpp 

```cpp
#include "ProtocolHelper.h"

#include <arpa/inet.h>

QByteArray ProtocolHelper::createMessage(MessageType type, const QByteArray& body) {
    MessageHeader header;
    header.type = type;
    header.body_len = htonl(static_cast<uint32_t>(body.size()));
    
    QByteArray message;
    message.append(reinterpret_cast<const char*>(&header), sizeof(MessageHeader));
    message.append(body);
    
    return message;
}

void ProtocolHelper::copyToFixedArray(char* dest, int maxLen, const QString& src) {
    QByteArray utf8 = src.toUtf8();
    int copyLen = qMin(maxLen - 1, utf8.size());
    std::memcpy(dest, utf8.constData(), copyLen);
    dest[copyLen] = '\0';
}

QString ProtocolHelper::fromFixedArray(const char* src, int maxLen) {
    int len = 0;
    while (len < maxLen && src[len] != '\0') {
        len++;
    }
    return QString::fromUtf8(src, len);
}

uint32_t ProtocolHelper::htonl(uint32_t hostlong) {
    return ::htonl(hostlong);
}

uint32_t ProtocolHelper::ntohl(uint32_t netlong) {
    return ::ntohl(netlong);
}

uint16_t ProtocolHelper::htons(uint16_t hostshort) {
    return ::htons(hostshort);
}

uint16_t ProtocolHelper::ntohs(uint16_t netshort) {
    return ::ntohs(netshort);
}

```

---

##  network/PosixSocketClient.h 

```cpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

class PosixSocketClient {
public:
    PosixSocketClient();
    ~PosixSocketClient();

    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const;

    bool send(const uint8_t* data, size_t len);
    int recv(uint8_t* buffer, size_t maxLen);

    int getFd() const { return m_socketFd; }

private:
    int m_socketFd;
};

```

---

##  ui/LobbyWindow.cpp 

```cpp
#include "LobbyWindow.h"
#include "ui_LobbyWindow.h"
#include "CreateRoomDialog.h"
#include "RoomWindow.h"
#include "StatsWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>

LobbyWindow::LobbyWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LobbyWindow)
    , networkManager(&NetworkManager::instance())
    , roomWindow(nullptr)
    , statsWindow(nullptr)
    , refreshTimer(new QTimer(this))
    , selectedRoomIndex(-1)
    , joinInProgress(false)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    startAutoRefresh();

    // Initial load
    onRefreshTimer();
}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::setupUI() {
    // Update welcome label
    auto &session = SessionState::instance();
    ui->lblUserInfo->setText(QString("Welcome, %1").arg(session.getDisplayName()));

    // Configure table widget
    ui->tblRooms->setColumnCount(6);
    ui->tblRooms->setHorizontalHeaderLabels(
        {"Room ID", "Room Name", "Players", "Mode", "Status", "Action"});
    ui->tblRooms->setColumnWidth(0, 70);
    ui->tblRooms->setColumnWidth(1, 180);
    ui->tblRooms->setColumnWidth(2, 80);
    ui->tblRooms->setColumnWidth(3, 100);
    ui->tblRooms->setColumnWidth(4, 100);
    ui->tblRooms->setColumnWidth(5, 80);
}

void LobbyWindow::setupConnections() {
    // UI signals
    connect(ui->btnRefresh, &QPushButton::clicked, this, &LobbyWindow::onRefreshClicked);
    connect(ui->btnCreateRoom, &QPushButton::clicked, this, &LobbyWindow::onCreateRoomClicked);
    connect(ui->btnStats, &QPushButton::clicked, this, &LobbyWindow::onStatsClicked);
    connect(ui->btnLogout, &QPushButton::clicked, this, &LobbyWindow::onLogoutClicked);
    // Table click - connect once here (avoid reconnecting on every populate)
    connect(ui->tblRooms, &QTableWidget::cellClicked, this, &LobbyWindow::onRoomTableItemClicked);

    // Network signals
    connect(networkManager, &NetworkManager::listRoomsResponse,
            this, &LobbyWindow::onListRoomsResponse);
    connect(networkManager, &NetworkManager::createRoomResponse,
            this, &LobbyWindow::onCreateRoomResponse);
    connect(networkManager, &NetworkManager::joinRoomResponse,
            this, &LobbyWindow::onJoinRoomResponse);
        connect(networkManager, &NetworkManager::statsResponse,
            this, &LobbyWindow::onStatsResponse);
        connect(networkManager, &NetworkManager::connectionError,
            this, &LobbyWindow::onConnectionError);

    // Timer
    connect(refreshTimer, &QTimer::timeout, this, &LobbyWindow::onRefreshTimer);
}

void LobbyWindow::startAutoRefresh() {
    refreshTimer->start(2000); // Refresh every 2 seconds
}

void LobbyWindow::stopAutoRefresh() {
    refreshTimer->stop();
}

void LobbyWindow::onRefreshClicked() {
    ui->lblStatus->setText("Loading rooms...");
    networkManager->sendListRooms();
}

void LobbyWindow::onCreateRoomClicked() {
    CreateRoomDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Dialog handles sending the create room request
        ui->lblStatus->setText("Creating room...");
    }
}

void LobbyWindow::onStatsClicked() {
    if (!statsWindow) {
        statsWindow = new StatsWindow(this);
    }

    statsWindow->showLoading();
    statsWindow->show();
    statsWindow->raise();
    statsWindow->activateWindow();

    networkManager->sendGetStats();
}

void LobbyWindow::onStatsResponse(const UserStatsResponse& stats) {
    if (!statsWindow) {
        statsWindow = new StatsWindow(this);
    }
    statsWindow->setStats(stats);
    statsWindow->show();
    statsWindow->raise();
    statsWindow->activateWindow();
}

void LobbyWindow::onLogoutClicked() {
    int ret = QMessageBox::question(this, "Confirm Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        SessionState::instance().clear();
        this->close();
        // LoginWindow will be shown by parent logic
    }
}

void LobbyWindow::onRoomTableItemClicked(int row, int column) {
    if (column == 5) { // Action column (Join button)
        if (row >= 0 && row < cachedRooms.size()) {
            // Prevent duplicate join attempts while one is in progress
            if (joinInProgress) {
                return;
            }
            selectedRoomIndex = row;
            RoomInfo room = cachedRooms[row];

            // If already in a room, prevent cross-room joins. If the selected room
            // is the one we're already in, simply open it without sending a join.
            auto &session = SessionState::instance();
            if (session.isInRoom()) {
                if (session.getCurrentRoomId() == room.room_id) {
                    ui->lblStatus->setText("Opening your room view...");
                    // Open RoomWindow using known room info; player list will populate via notifications
                    if (!roomWindow) {
                        roomWindow = new RoomWindow(room, /*host_user_id*/ session.getUserId(), QVector<PlayerInfo>{}, this);
                    }
                    this->hide();
                    roomWindow->show();
                    return;
                } else {
                    QMessageBox::information(this, "Already in a room",
                        "You are already in a room. Please leave it before joining another.");
                    return;
                }
            }
            ui->lblStatus->setText(QString("Joining room '%1'...").arg(room.room_name));

            // Send join request
            joinInProgress = true;
            networkManager->sendJoinRoom(room.room_id);
        }
    }
}

void LobbyWindow::onListRoomsResponse(uint16_t count, const QVector<RoomInfo>& rooms) {
    cachedRooms = rooms;
    populateRoomTable(rooms);

    if (count == 0) {
        ui->lblStatus->setText("No rooms available. Create a new one!");
    } else {
        ui->lblStatus->setText(QString("Showing %1 room(s)").arg(count));
    }
}

void LobbyWindow::onCreateRoomResponse(StatusCode code, const RoomInfo& room_info) {
    if (code == StatusCode::SUCCESS) { // Success
        QMessageBox::information(this, "Room Created",
            QString("Room '%1' created successfully!\nRoom ID: %2")
                .arg(room_info.room_name)
                .arg(room_info.room_id));
        // Host is placed into the room upon creation; track it
        SessionState::instance().setCurrentRoomId(room_info.room_id);
        onRefreshClicked(); // Refresh room list
    } else {
        QMessageBox::critical(this, "Create Room Failed",
            QString("Failed to create room. Error code: %1").arg((uint8_t)code));
    }
}

void LobbyWindow::onJoinRoomResponse(StatusCode code, const RoomInfo& room_info,
                                     uint8_t player_count, const QVector<PlayerInfo>& players,
                                     uint32_t host_user_id) {
    auto &session = SessionState::instance();
    const uint32_t myUserId = session.getUserId();

    if (code == StatusCode::SUCCESS) { // Success
        // Store room info in session
        session.setCurrentRoomId(room_info.room_id);

        QMessageBox::information(this, "Room Joined",
            QString("Joined room '%1'!\n"
                    "Players: %2")
                .arg(room_info.room_name)
                .arg(player_count));

        // Stop auto-refresh before transitioning away
        stopAutoRefresh();

        // Transition to RoomWindow (Phase 4)
        if (!roomWindow) {
            roomWindow = new RoomWindow(room_info, host_user_id, players, this);
        }
        this->hide();
        roomWindow->show();
        joinInProgress = false;
        return;
    }

    // Non-success path. If we are already in a room, silently open the current room view.
    if (session.isInRoom()) {
        const uint32_t currentRoomId = session.getCurrentRoomId();
        // Try to find the room info from the cached list
        RoomInfo currentRoomInfo{};
        bool found = false;
        for (const auto &r : cachedRooms) {
            if (r.room_id == currentRoomId) { currentRoomInfo = r; found = true; break; }
        }

        if (found) {
            ui->lblStatus->setText("Opening your room view...");
            stopAutoRefresh();
            if (!roomWindow) {
                roomWindow = new RoomWindow(currentRoomInfo, /*host_user_id*/ myUserId, QVector<PlayerInfo>{}, this);
            }
            this->hide();
            roomWindow->show();
            joinInProgress = false;
            return;
        }
    }

    // Otherwise, show error dialog only once if user initiated
    if (joinInProgress) {
        QMessageBox::critical(this, "Join Room Failed",
            QString("Failed to join room. Error code: %1").arg((uint8_t)code));
        joinInProgress = false;
    }
}

void LobbyWindow::onConnectionError(const QString &error) {
    ui->lblError->setText("Network error: " + error);
    ui->lblStatus->setText("Error loading rooms");
}

void LobbyWindow::onRefreshTimer() {
    networkManager->sendListRooms();
}

void LobbyWindow::populateRoomTable(const QVector<RoomInfo> &rooms) {
    ui->tblRooms->setRowCount(0);

    for (int i = 0; i < rooms.size(); ++i) {
        const RoomInfo &room = rooms[i];

        int row = ui->tblRooms->rowCount();
        ui->tblRooms->insertRow(row);

        // Room ID
        auto *itemId = new QTableWidgetItem(QString::number(room.room_id));
        itemId->setFlags(itemId->flags() & ~Qt::ItemIsEditable);
        ui->tblRooms->setItem(row, 0, itemId);

        // Room Name (bounded conversion from fixed-size char array)
        auto *itemName = new QTableWidgetItem(QString::fromLatin1(room.room_name, sizeof(room.room_name)));
        itemName->setFlags(itemName->flags() & ~Qt::ItemIsEditable);
        ui->tblRooms->setItem(row, 1, itemName);

        // Players
        QString playerCount = QString("%1/%2")
            .arg(room.current_players)
            .arg(room.max_players);
        auto *itemPlayers = new QTableWidgetItem(playerCount);
        itemPlayers->setFlags(itemPlayers->flags() & ~Qt::ItemIsEditable);
        ui->tblRooms->setItem(row, 2, itemPlayers);

        // Mode
        auto *itemMode = new QTableWidgetItem(formatGameMode(room.game_mode));
        itemMode->setFlags(itemMode->flags() & ~Qt::ItemIsEditable);
        ui->tblRooms->setItem(row, 3, itemMode);

        // Status
        auto *itemStatus = new QTableWidgetItem(formatRoomStatus(room.is_in_game));
        itemStatus->setFlags(itemStatus->flags() & ~Qt::ItemIsEditable);
        ui->tblRooms->setItem(row, 4, itemStatus);

        // Action (Join button)
        auto *itemAction = new QTableWidgetItem("Join");
        itemAction->setFlags(itemAction->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tblRooms->setItem(row, 5, itemAction);
    }

    // Table click signal is connected once in setupConnections()
}

QString LobbyWindow::formatGameMode(GameMode mode) const {
    switch (mode) {
        case GameMode::ELIMINATION:
            return "Elimination";
        case GameMode::SCORING:
            return "Scoring";
        default:
            return "Unknown";
    }
}

QString LobbyWindow::formatRoomStatus(bool inGame) const {
    return inGame ? "In Game" : "Waiting";
}

```

---

##  ui/LoginWindow.h 

```cpp
#pragma once

#include <QMainWindow>
#include "protocol.h"

class MainMenuWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow {
    Q_OBJECT
    
public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    
private slots:
    void onLoginClicked();
    void onRegisterClicked();
    
    void onConnected();
    void onDisconnected();
    void onConnectionError(const QString& error);
    void onRegisterResponse(StatusCode code);
    void onLoginResponse(StatusCode code, uint32_t userId, const QString& displayName);
    
private:
    void connectToServer();
    bool validateLoginInput();
    bool validateRegisterInput();
    void showError(const QString& message);
    void clearError();
    
    Ui::LoginWindow *ui;
    MainMenuWindow *mainMenuWindow;
};

```

---

##  ui/LobbyWindow.h 

```cpp
#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "protocol.h"

class NetworkManager;
class RoomWindow;
class StatsWindow;

namespace Ui {
class LobbyWindow;
}

class LobbyWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LobbyWindow(QWidget *parent = nullptr);
    ~LobbyWindow();

private slots:
    // UI slots
    void onRefreshClicked();
    void onCreateRoomClicked();
    void onStatsClicked();
    void onLogoutClicked();
    void onRoomTableItemClicked(int row, int column);

    // Network slots
    void onListRoomsResponse(uint16_t count, const QVector<RoomInfo>& rooms);
    void onCreateRoomResponse(StatusCode code, const RoomInfo& room_info);
    void onJoinRoomResponse(StatusCode code, const RoomInfo& room_info, 
                            uint8_t player_count, const QVector<PlayerInfo>& players, uint32_t host_user_id);
    void onConnectionError(const QString &error);
    void onStatsResponse(const UserStatsResponse& stats);

    // Timer slot
    void onRefreshTimer();

private:
    void setupUI();
    void setupConnections();
    void startAutoRefresh();
    void stopAutoRefresh();
    void populateRoomTable(const QVector<RoomInfo> &rooms);
    void addJoinButtonToRow(int row);
    QString formatGameMode(GameMode mode) const;
    QString formatRoomStatus(bool inGame) const;

    Ui::LobbyWindow *ui;
    NetworkManager *networkManager;
    RoomWindow *roomWindow;
    StatsWindow *statsWindow;
    QTimer *refreshTimer;
    QVector<RoomInfo> cachedRooms;
    int selectedRoomIndex;
    bool joinInProgress;
};

#endif // LOBBYWINDOW_H

```

---

##  ui/MainMenuWindow.h 

```cpp
#pragma once

#include <QMainWindow>
#include <QVector>
#include "protocol.h"

namespace Ui {
class MainMenuWindow;
}

class NetworkManager;

class MainMenuWindow : public QMainWindow {
    Q_OBJECT

public:
    MainMenuWindow(QWidget* parent = nullptr);
    ~MainMenuWindow();

private slots:
    void onStatsReceived(const UserStatsResponse& stats);
    void onPlayClicked();
    void onViewStatsClicked();
    void onReplayClicked();
    void onGameHistoryClicked();
    void onConnectionError(const QString& error);
    void loadStats();

private:
    void setupUI();
    void displayStats(const UserStatsResponse& stats);
    QString getTierName(uint32_t rankedPoints) const;

    Ui::MainMenuWindow* ui;
    NetworkManager* networkManager;
    UserStatsResponse currentStats;
};

```

---

##  ui/StatsWindow.h 

```cpp
#pragma once

#include <QDialog>
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StatsWindow; }
QT_END_NAMESPACE

class StatsWindow : public QDialog {
    Q_OBJECT

public:
    explicit StatsWindow(QWidget *parent = nullptr);
    ~StatsWindow();

    void setStats(const UserStatsResponse &stats);
    void showLoading();

private:
    Ui::StatsWindow *ui;
};

```

---

##  ui/GameWindow.cpp 

```cpp
#include "ui_GameWindow.h"
#include "GameWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"

#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPalette>
#include <algorithm>

namespace {
QString fromFixed(const char* buf, size_t len) {
    return QString::fromLatin1(buf, static_cast<int>(len)).split('\0').first();
}
}

GameWindow::GameWindow(GameMode mode, uint32_t roomId, uint32_t hostUserId,
                       const QVector<PlayerInfo>& initialPlayers, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , networkManager(&NetworkManager::instance())
    , sessionState(&SessionState::instance())
    , gameMode(mode)
    , roomId(roomId)
    , hostUserId(hostUserId)
    , questionTimer(new QTimer(this))
    , timeRemainingSec(0)
    , currentQuestionId(0)
    , questionNumber(0)
    , questionActive(false)
    , answerSent(false)
    , paused(false)
{
    ui->setupUi(this);
    setupUiTheme();
    bindSignals();

    // Seed players list
    players.clear();
    players.reserve(initialPlayers.size());
    for (const auto& p : initialPlayers) {
        ClientPlayer cp;
        cp.user_id = p.user_id;
        cp.display_name = fromFixed(p.display_name, sizeof(p.display_name));
        cp.is_ready = p.is_ready;
        cp.score = 0;
        cp.is_eliminated = false;
        players.append(cp);
    }

    // Header info
    ui->lblRoomTitle->setText(QString("Room: %1").arg(roomId));
    ui->lblMode->setText(QString("Mode: %1").arg(gameMode == GameMode::ELIMINATION ? "Elimination" : "Scoring"));
    ui->lblQuestionCounter->setText("Question 0/0");
    ui->lblTimer->setText("Timer: --");
    ui->progressTimer->setMaximum(100);
    ui->progressTimer->setValue(0);
    ui->lblFeedback->setText("Waiting for game start...");

    // Scoreboard setup
    ui->tblScoreboard->setColumnCount(4);
    ui->tblScoreboard->setHorizontalHeaderLabels({"Rank", "Player", "Score", "Status"});
    ui->tblScoreboard->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblScoreboard->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblScoreboard->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblScoreboard->setSelectionMode(QAbstractItemView::NoSelection);
    updateScoreboard();

    // Disable answer buttons until a question arrives
    setButtonsEnabled(false);

    connect(questionTimer, &QTimer::timeout, this, &GameWindow::onQuestionTick);
}

GameWindow::~GameWindow() {
    stopQuestionTimer();
    delete ui;
}

void GameWindow::setupUiTheme() {
    // Simple Kahoot-like color blocks for the four buttons
    ui->btnOptionA->setStyleSheet("background:#e74c3c; color:white; font-size:16px; font-weight:bold;");
    ui->btnOptionB->setStyleSheet("background:#2980b9; color:white; font-size:16px; font-weight:bold;");
    ui->btnOptionC->setStyleSheet("background:#f1c40f; color:black; font-size:16px; font-weight:bold;");
    ui->btnOptionD->setStyleSheet("background:#27ae60; color:white; font-size:16px; font-weight:bold;");
}

void GameWindow::bindSignals() {
    connect(ui->btnOptionA, &QPushButton::clicked, this, &GameWindow::onOptionAClicked);
    connect(ui->btnOptionB, &QPushButton::clicked, this, &GameWindow::onOptionBClicked);
    connect(ui->btnOptionC, &QPushButton::clicked, this, &GameWindow::onOptionCClicked);
    connect(ui->btnOptionD, &QPushButton::clicked, this, &GameWindow::onOptionDClicked);
    connect(ui->btnPauseGame, &QPushButton::clicked, this, &GameWindow::onPauseGameClicked);
    connect(ui->btnResumeGame, &QPushButton::clicked, this, &GameWindow::onResumeGameClicked);

    // Network signals
    connect(networkManager, &NetworkManager::questionNotif,
            this, &GameWindow::onQuestionReceived);
    connect(networkManager, &NetworkManager::roundResultNotif,
            this, &GameWindow::onRoundResult);
    connect(networkManager, &NetworkManager::playerEliminatedNotif,
            this, &GameWindow::onPlayerEliminated);
    connect(networkManager, &NetworkManager::gameOverNotif,
            this, &GameWindow::onGameOver);
    connect(networkManager, &NetworkManager::gamePausedNotif,
            this, &GameWindow::onGamePaused);
    connect(networkManager, &NetworkManager::gameResumedNotif,
            this, &GameWindow::onGameResumed);
    connect(networkManager, &NetworkManager::gameTerminatedNotif,
            this, &GameWindow::onGameTerminated);
    connect(networkManager, &NetworkManager::connectionError,
            this, &GameWindow::onConnectionError);
    connect(networkManager, &NetworkManager::returnToRoomResponse,
            this, [this](StatusCode code) {
                if (code == StatusCode::SUCCESS) {
                    emit returnedToRoom();
                }
            });
}

void GameWindow::setButtonsEnabled(bool enabled) {
    ui->btnOptionA->setEnabled(enabled && !paused);
    ui->btnOptionB->setEnabled(enabled && !paused);
    ui->btnOptionC->setEnabled(enabled && !paused);
    ui->btnOptionD->setEnabled(enabled && !paused);
}

void GameWindow::onPauseGameClicked() {
    if (!paused && hostUserId == sessionState->getUserId()) {
        networkManager->sendPauseGame();
    }
}

void GameWindow::onResumeGameClicked() {
    if (paused && hostUserId == sessionState->getUserId()) {
        networkManager->sendResumeGame();
    }
}

void GameWindow::startQuestion(uint32_t questionId, const QString& content,
                               const QStringList& options, uint32_t timeLimitSec) {
    questionActive = true;
    answerSent = false;
    currentQuestionId = questionId;
    questionNumber += 1;

    ui->lblQuestion->setText(content);
    ui->lblQuestionCounter->setText(QString("Question %1").arg(questionNumber));
    ui->lblFeedback->setText("Pick an answer before time runs out.");

    ui->btnOptionA->setText(QString("A) %1").arg(options.value(0)));
    ui->btnOptionB->setText(QString("B) %1").arg(options.value(1)));
    ui->btnOptionC->setText(QString("C) %1").arg(options.value(2)));
    ui->btnOptionD->setText(QString("D) %1").arg(options.value(3)));

    resetOptionStyles();
    setButtonsEnabled(true);

    startQuestionTimer(timeLimitSec);
    answerElapsed.restart();
}

void GameWindow::startQuestionTimer(uint32_t seconds) {
    timeRemainingSec = static_cast<int>(seconds);
    if (timeRemainingSec < 0) timeRemainingSec = 0;
    ui->lblTimer->setText(QString("Timer: %1").arg(timeRemainingSec));
    ui->progressTimer->setMaximum(std::max(1, timeRemainingSec));
    ui->progressTimer->setValue(timeRemainingSec);

    questionTimer->setInterval(1000);
    questionTimer->start();
}

void GameWindow::stopQuestionTimer() {
    if (questionTimer) {
        questionTimer->stop();
    }
}

void GameWindow::updateTimerUi() {
    ui->lblTimer->setText(QString("Timer: %1").arg(timeRemainingSec));
    ui->progressTimer->setValue(std::max(0, timeRemainingSec));
}

void GameWindow::onQuestionReceived(uint32_t questionId, const QString& content,
                                    const QStringList& options, uint32_t timeLimitSec) {
    // Reset paused banner
    ui->lblGameStatus->clear();
    paused = false;
    setButtonsEnabled(true);

    startQuestion(questionId, content, options, timeLimitSec);
}

void GameWindow::handleAnswerSelection(int optionIndex) {
    if (!questionActive || answerSent || paused) return;
    answerSent = true;
    setButtonsEnabled(false);

    // optionIndex is 1-based for protocol compatibility
    qDebug().nospace() << "Player " << sessionState->getUserId()
                       << " selected answer " << optionIndex
                       << " for Question " << currentQuestionId;
    uint32_t elapsedMs = static_cast<uint32_t>(answerElapsed.elapsed());
    networkManager->sendSubmitAnswer(currentQuestionId, static_cast<uint8_t>(optionIndex), elapsedMs);

    markSelection(optionIndex);
    setFeedback("Answer submitted. Waiting for results...");
}

void GameWindow::onOptionAClicked() { handleAnswerSelection(1); }
void GameWindow::onOptionBClicked() { handleAnswerSelection(2); }
void GameWindow::onOptionCClicked() { handleAnswerSelection(3); }
void GameWindow::onOptionDClicked() { handleAnswerSelection(4); }

void GameWindow::onQuestionTick() {
    if (timeRemainingSec > 0) {
        timeRemainingSec -= 1;
        updateTimerUi();
    }
    if (timeRemainingSec <= 0) {
        stopQuestionTimer();
        questionActive = false;
        setButtonsEnabled(false);
        setFeedback("Time's up. Waiting for results...");
    }
}

void GameWindow::markSelection(int optionIndex) {
    QPushButton* buttons[4] = {ui->btnOptionA, ui->btnOptionB, ui->btnOptionC, ui->btnOptionD};
    for (int i = 0; i < 4; ++i) {
        if (i == optionIndex - 1) {
            buttons[i]->setStyleSheet(buttons[i]->styleSheet() + "border: 3px solid #2c3e50;");
        }
    }
}

void GameWindow::highlightCorrect(uint8_t correctOption) {
    QPushButton* buttons[4] = {ui->btnOptionA, ui->btnOptionB, ui->btnOptionC, ui->btnOptionD};
    for (int i = 0; i < 4; ++i) {
        QString base = buttons[i]->styleSheet();
        if (i == static_cast<int>(correctOption) - 1) {
            buttons[i]->setStyleSheet(base + "border: 3px solid #27ae60;");
        } else if (answerSent && i != static_cast<int>(correctOption) - 1) {
            buttons[i]->setStyleSheet(base + "opacity: 0.6;");
        }
    }
}

void GameWindow::resetOptionStyles() {
    setupUiTheme();
}

void GameWindow::applyRoundResults(const QVector<PlayerRoundResult>& results) {
    for (const auto& res : results) {
        for (auto& p : players) {
            if (p.user_id == res.user_id) {
                p.score = res.total_score;
                p.is_eliminated = res.was_eliminated;
                break;
            }
        }
    }
    updateScoreboard();
}

void GameWindow::onRoundResult(uint8_t correctOption, uint8_t /*playerCount*/,
                               const QVector<PlayerRoundResult>& results) {
    questionActive = false;
    stopQuestionTimer();
    highlightCorrect(correctOption == 0 ? 1 : correctOption);
    applyRoundResults(results);

    QString msg = QString("Correct answer: %1").arg(correctOption);
    setFeedback(msg);
}

void GameWindow::onPlayerEliminated(uint32_t userId) {
    for (auto& p : players) {
        if (p.user_id == userId) {
            p.is_eliminated = true;
            break;
        }
    }
    updateScoreboard();
}

void GameWindow::onGameOver(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings, 
                            uint8_t gameEndReason, uint8_t winnerCount) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    questionActive = false;

    QString text;
    
    // 0 = SINGLE_WINNER, 1 = DRAW, 2 = NO_WINNER_WIPEOUT
    if (gameEndReason == 2) {
        // No Winner - Everyone eliminated
        text = "Game Over! No Winner - Everyone answered the last question incorrectly!";
    } else if (gameEndReason == 1) {
        // Draw - Multiple winners
        QStringList winners;
        for (const auto& p : rankings) {
            if (p.is_winner) {
                winners.append(QString::fromLatin1(p.display_name, MAX_DISPLAY_NAME_LEN));
            }
        }
        text = QString("Game Over! It's a Draw!\nWinners: %1").arg(winners.join(", "));
    } else {
        // Single Winner
        QString winner = rankingCount > 0 ? QString::fromLatin1(rankings[0].display_name, MAX_DISPLAY_NAME_LEN) : "";
        text = QString("Game over!%1").arg(winner.isEmpty() ? QString() : QString(" Winner: %1").arg(winner));
    }
    
    QMessageBox::information(this, "Game Over", text);

    // Send return to room request instead of closing
    networkManager->sendReturnToRoom();
}

void GameWindow::onGamePaused() {
    paused = true;
    stopQuestionTimer();
    setButtonsEnabled(false);
    ui->lblGameStatus->setText("Game paused");
    ui->btnPauseGame->setVisible(false);
    ui->btnResumeGame->setVisible(true);
}

void GameWindow::onGameResumed() {
    paused = false;
    ui->lblGameStatus->clear();
    ui->btnPauseGame->setVisible(true);
    ui->btnResumeGame->setVisible(false);
    if (questionActive && timeRemainingSec > 0) {
        setButtonsEnabled(true);
        questionTimer->start();
    }
}

void GameWindow::onGameTerminated(TerminationReason reason) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    QString why;
    switch (reason) {
        case TerminationReason::HOST_LEFT: why = "Host left the match"; break;
        case TerminationReason::NOT_ENOUGH_PLAYERS: why = "Not enough players"; break;
        case TerminationReason::SERVER_SHUTDOWN: why = "Server shutdown"; break;
        default: why = "Match terminated"; break;
    }
    QMessageBox::warning(this, "Game Terminated", why);
    sessionState->setCurrentRoomId(0);
    close();
}

void GameWindow::onConnectionError(const QString& error) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    QMessageBox::critical(this, "Connection Error", error);
}

void GameWindow::onReturnedToRoom() {
    // No-op: RoomWindow handles closing this window and showing room view
}

void GameWindow::updateScoreboard() {
    // Sort players by score descending, non-eliminated first
    QVector<ClientPlayer> sorted = players;
    std::sort(sorted.begin(), sorted.end(), [](const ClientPlayer& a, const ClientPlayer& b) {
        if (a.is_eliminated != b.is_eliminated) return !a.is_eliminated && b.is_eliminated;
        return a.score > b.score;
    });

    ui->tblScoreboard->setRowCount(0);
    for (int i = 0; i < sorted.size(); ++i) {
        const auto& p = sorted[i];
        int row = ui->tblScoreboard->rowCount();
        ui->tblScoreboard->insertRow(row);

        auto *rankItem = new QTableWidgetItem(QString::number(i + 1));
        auto *nameItem = new QTableWidgetItem(p.display_name);
        auto *scoreItem = new QTableWidgetItem(QString::number(p.score));
        auto *statusItem = new QTableWidgetItem(formatStatus(p));

        rankItem->setFlags(rankItem->flags() & ~Qt::ItemIsEditable);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        scoreItem->setFlags(scoreItem->flags() & ~Qt::ItemIsEditable);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);

        // Highlight current user for clarity (bold + subtle background)
        if (p.user_id == sessionState->getUserId()) {
            QFont bold;
            bold.setBold(true);
            rankItem->setFont(bold);
            nameItem->setFont(bold);
            scoreItem->setFont(bold);
            statusItem->setFont(bold);

            QColor bg(230, 240, 255); // soft blue tint
            rankItem->setBackground(QBrush(bg));
            nameItem->setBackground(QBrush(bg));
            scoreItem->setBackground(QBrush(bg));
            statusItem->setBackground(QBrush(bg));
        }

        ui->tblScoreboard->setItem(row, 0, rankItem);
        ui->tblScoreboard->setItem(row, 1, nameItem);
        ui->tblScoreboard->setItem(row, 2, scoreItem);
        ui->tblScoreboard->setItem(row, 3, statusItem);
    }
}

QString GameWindow::formatStatus(const ClientPlayer& p) const {
    QStringList bits;
    if (p.user_id == hostUserId) bits << "Host";
    if (p.user_id == sessionState->getUserId()) bits << "You";
    if (p.is_eliminated) bits << "Eliminated";
    return bits.isEmpty() ? "Player" : bits.join(", ");
}

void GameWindow::setFeedback(const QString& text, bool warn) {
    ui->lblFeedback->setText(text);
    ui->lblFeedback->setStyleSheet(warn ? "color:#c0392b;" : "");
}

```

---

##  ui/CreateRoomDialog.cpp 

```cpp
#include "CreateRoomDialog.h"
#include "ui_CreateRoomDialog.h"
#include "../network/NetworkManager.h"
#include <QMessageBox>

CreateRoomDialog::CreateRoomDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateRoomDialog)
    , networkManager(&NetworkManager::instance())
{
    ui->setupUi(this);
    setupConnections();
}

CreateRoomDialog::~CreateRoomDialog() {
    delete ui;
}

void CreateRoomDialog::setupConnections() {
    connect(ui->btnCreate, &QPushButton::clicked, this, &CreateRoomDialog::onCreateClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &CreateRoomDialog::onCancelClicked);

        connect(networkManager, &NetworkManager::createRoomResponse,
            this, &CreateRoomDialog::onCreateRoomResponse);
        connect(networkManager, &NetworkManager::connectionError,
            this, &CreateRoomDialog::onConnectionError);
}

void CreateRoomDialog::onCreateClicked() {
    if (!validateInput()) {
        return;
    }

    QString roomName = ui->edtRoomName->text().trimmed();
    GameMode gameMode = static_cast<GameMode>(ui->cmbGameMode->currentIndex());
    uint8_t questionCount = static_cast<uint8_t>(ui->spinQuestionCount->value());

    networkManager->sendCreateRoom(roomName, gameMode, questionCount);
    this->accept();
}

void CreateRoomDialog::onCancelClicked() {
    this->reject();
}

void CreateRoomDialog::onCreateRoomResponse(StatusCode code, const RoomInfo& room_info) {
    if (code == StatusCode::SUCCESS) { // Success
        QMessageBox::information(this, "Success",
            QString("Room '%1' created successfully!").arg(room_info.room_name));
    } else {
        QMessageBox::critical(this, "Error",
            QString("Failed to create room. Error code: %1").arg((uint8_t)code));
    }
}

void CreateRoomDialog::onConnectionError(const QString &error) {
    ui->lblError->setText("Network error: " + error);
    QMessageBox::critical(this, "Network Error", error);
}

bool CreateRoomDialog::validateInput() {
    QString roomName = ui->edtRoomName->text().trimmed();

    if (roomName.isEmpty()) {
        ui->lblError->setText("Room name cannot be empty!");
        return false;
    }

    if (roomName.length() > 32) {
        ui->lblError->setText("Room name cannot exceed 32 characters!");
        return false;
    }

    if (ui->spinQuestionCount->value() < 5 || ui->spinQuestionCount->value() > 20) {
        ui->lblError->setText("Question count must be between 5 and 20!");
        return false;
    }

    ui->lblError->setText("");
    return true;
}

```

---

##  ui/StatsWindow.cpp 

```cpp
#include "StatsWindow.h"
#include "ui_StatsWindow.h"
#include <QString>
#include <QtGlobal>

static QString formatPercent(double value) {
    return QString::number(value, 'f', 1) + "%";
}

StatsWindow::StatsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    showLoading();
}

StatsWindow::~StatsWindow() {
    delete ui;
}

void StatsWindow::showLoading() {
    const auto resetMode = [](QLabel* matches, QLabel* wins, QLabel* winRate,
                               QLabel* correct, QLabel* incorrect, QLabel* avgOrHigh = nullptr,
                               QLabel* highScore = nullptr) {
        matches->setText("Loading...");
        wins->setText("-");
        winRate->setText("-");
        correct->setText("-");
        incorrect->setText("-");
        if (avgOrHigh) avgOrHigh->setText("-");
        if (highScore) highScore->setText("-");
    };

    resetMode(ui->lblElimMatchesValue, ui->lblElimWinsValue, ui->lblElimWinRateValue,
              ui->lblElimCorrectValue, ui->lblElimIncorrectValue);

    resetMode(ui->lblScoreMatchesValue, ui->lblScoreWinsValue, ui->lblScoreWinRateValue,
              ui->lblScoreCorrectValue, ui->lblScoreIncorrectValue,
              ui->lblScoreAverageValue, ui->lblScoreHighValue);

    ui->lblRankedPointsValue->setText("-");
}

void StatsWindow::setStats(const UserStatsResponse &stats) {
    auto updateMode = [](const UserModeStats& mode, QLabel* matches, QLabel* wins,
                         QLabel* winRate, QLabel* correct, QLabel* incorrect,
                         QLabel* averageScore = nullptr, QLabel* highScore = nullptr) {
        const double rate = (mode.total_matches == 0)
                                ? 0.0
                                : (static_cast<double>(mode.wins) * 100.0 / mode.total_matches);

        matches->setText(QString::number(mode.total_matches));
        wins->setText(QString::number(mode.wins));
        winRate->setText(formatPercent(rate));
        correct->setText(QString::number(mode.total_correct_answers));
        incorrect->setText(QString::number(mode.total_incorrect_answers));
        if (averageScore) {
            averageScore->setText(QString::number(mode.average_score, 'f', 1));
        }
        if (highScore) {
            highScore->setText(QString::number(mode.high_score));
        }
    };

    updateMode(stats.elimination, ui->lblElimMatchesValue, ui->lblElimWinsValue,
               ui->lblElimWinRateValue, ui->lblElimCorrectValue, ui->lblElimIncorrectValue);

    updateMode(stats.scoring, ui->lblScoreMatchesValue, ui->lblScoreWinsValue,
               ui->lblScoreWinRateValue, ui->lblScoreCorrectValue, ui->lblScoreIncorrectValue,
               ui->lblScoreAverageValue, ui->lblScoreHighValue);

    ui->lblRankedPointsValue->setText(QString::number(stats.ranked_points));
}

```

---

##  ui/CreateRoomDialog.h 

```cpp
#ifndef CREATEROCOMDIALOG_H
#define CREATEROCOMDIALOG_H

#include <QDialog>
#include "protocol.h"

class NetworkManager;

namespace Ui {
class CreateRoomDialog;
}

class CreateRoomDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateRoomDialog(QWidget *parent = nullptr);
    ~CreateRoomDialog();

private slots:
    void onCreateClicked();
    void onCancelClicked();
    void onCreateRoomResponse(StatusCode code, const RoomInfo& room_info);
    void onConnectionError(const QString &error);

private:
    void setupConnections();
    bool validateInput();

    Ui::CreateRoomDialog *ui;
    NetworkManager *networkManager;
};

#endif // CREATEROCOMDIALOG_H

```

---

##  ui/ReplayWindow.cpp 

```cpp
#include "ReplayWindow.h"
#include "ui_ReplayWindow.h"
#include "../network/NetworkManager.h"
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>

namespace {
QString formatTime(uint32_t ms) {
    uint32_t sec = ms / 1000;
    uint32_t msec = ms % 1000;
    return QString("%1.%2s").arg(sec).arg(msec, 3, 10, QChar('0'));
}
}

ReplayWindow::ReplayWindow(uint32_t sessionId, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ReplayWindow)
    , networkManager(&NetworkManager::instance())
    , sessionId(sessionId)
    , gameMode(GameMode::ELIMINATION)
    , currentEventIndex(0)
    , isPlaying(false)
    , playbackTimer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle(QString("Replay - Session %1").arg(sessionId));
    
    connect(playbackTimer, &QTimer::timeout, this, &ReplayWindow::onReplayTick);
    connect(networkManager, &NetworkManager::replayDataResponse,
            this, &ReplayWindow::onReplayDataReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &ReplayWindow::onConnectionError);
    
    ui->btnPlay->setStyleSheet("background:#27ae60; color:white; font-weight:bold;");
    ui->btnPause->setStyleSheet("background:#f39c12; color:white; font-weight:bold;");
    ui->btnStop->setStyleSheet("background:#c0392b; color:white; font-weight:bold;");
    ui->btnNext->setStyleSheet("background:#3498db; color:white; font-weight:bold;");
    ui->btnPrev->setStyleSheet("background:#9b59b6; color:white; font-weight:bold;");
    
    connect(ui->btnPlay, &QPushButton::clicked, this, &ReplayWindow::onPlayClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &ReplayWindow::onPauseClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &ReplayWindow::onStopClicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &ReplayWindow::onNextEventClicked);
    connect(ui->btnPrev, &QPushButton::clicked, this, &ReplayWindow::onPrevEventClicked);
    
    ui->tblAnswers->setColumnCount(3);
    ui->tblAnswers->setHorizontalHeaderLabels({"Player", "Answer", "Response Time"});
    ui->tblAnswers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblAnswers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    ui->lblEventInfo->setText("Waiting for replay data...");
    
    loadReplayData();
}

ReplayWindow::~ReplayWindow() {
    delete ui;
}

void ReplayWindow::loadReplayData() {
    networkManager->sendGetReplay(sessionId);
}

void ReplayWindow::onReplayDataReceived(StatusCode status, uint32_t responseSessionId, 
                                        GameMode mode, const QVector<ReplayEvent>& eventList) {
    if (status != StatusCode::SUCCESS || responseSessionId != sessionId) {
        QMessageBox::critical(this, "Replay Error", "Failed to load replay data");
        return;
    }
    
    gameMode = mode;
    events = eventList;
    
    buildEventLog();
    updateEventList();
    
    if (!events.isEmpty()) {
        currentEventIndex = 0;
        displayCurrentEvent();
    }
}

void ReplayWindow::buildEventLog() {
    questions.clear();
    
    for (const auto& event : events) {
        QuestionData& qdata = questions[event.question_id];
        if (qdata.options.isEmpty()) {
            qdata.questionId = event.question_id;
        }
        
        qdata.playerAnswers[event.user_id] = event.selected_option;
        qdata.playerResponseTimes[event.user_id] = event.response_time_ms;
    }
}

void ReplayWindow::updateEventList() {
    ui->lstEvents->clear();
    
    for (int i = 0; i < events.size(); ++i) {
        const auto& event = events[i];
        QString text = QString("Event %1: Q%2 at %3ms")
            .arg(i + 1)
            .arg(event.question_id)
            .arg(event.timestamp_ms);
        ui->lstEvents->addItem(text);
    }
}

void ReplayWindow::displayCurrentEvent() {
    if (currentEventIndex < 0 || currentEventIndex >= events.size()) {
        ui->lblEventInfo->setText("No events to display");
        return;
    }
    
    const auto& event = events[currentEventIndex];
    
    QString difficultyStr;
    if (event.difficulty == 1) difficultyStr = "Easy";
    else if (event.difficulty == 2) difficultyStr = "Medium";
    else if (event.difficulty == 3) difficultyStr = "Hard";
    else difficultyStr = "Unknown";
    
    QString correctOptionStr = QString("Option %1").arg(QChar('A' + event.correct_option - 1));
    QString selectedOptionStr = QString("Option %1").arg(QChar('A' + event.selected_option - 1));
    bool isCorrect = (event.selected_option == event.correct_option);
    
    ui->lblEventInfo->setText(QString("Event %1 / %2 - %3 (%4) - Answered: %5 (Correct: %6) - %7ms")
        .arg(currentEventIndex + 1)
        .arg(events.size())
        .arg(QString::fromUtf8(event.question_content, strlen(event.question_content)))
        .arg(difficultyStr)
        .arg(selectedOptionStr)
        .arg(isCorrect ? "✓" : "✗")
        .arg(event.response_time_ms));
    
    ui->lstEvents->setCurrentRow(currentEventIndex);
    
    const auto& qdata = questions[event.question_id];
    ui->tblAnswers->setRowCount(0);
    
    int row = 0;
    for (auto it = qdata.playerAnswers.begin(); it != qdata.playerAnswers.end(); ++it) {
        uint32_t userId = it.key();
        uint8_t answer = it.value();
        uint32_t responseTime = qdata.playerResponseTimes.value(userId, 0);
        
        ui->tblAnswers->insertRow(row);
        
        auto* userItem = new QTableWidgetItem(QString::number(userId));
        auto* ansItem = new QTableWidgetItem(QString("Option %1").arg(QChar('A' + answer - 1)));
        auto* timeItem = new QTableWidgetItem(formatTime(responseTime));
        
        userItem->setFlags(userItem->flags() & ~Qt::ItemIsEditable);
        ansItem->setFlags(ansItem->flags() & ~Qt::ItemIsEditable);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        
        ui->tblAnswers->setItem(row, 0, userItem);
        ui->tblAnswers->setItem(row, 1, ansItem);
        ui->tblAnswers->setItem(row, 2, timeItem);
        
        ++row;
    }
}

void ReplayWindow::onPlayClicked() {
    if (!isPlaying) {
        playReplay();
    }
}

void ReplayWindow::onPauseClicked() {
    if (isPlaying) {
        pauseReplay();
    }
}

void ReplayWindow::onStopClicked() {
    stopReplay();
}

void ReplayWindow::onNextEventClicked() {
    if (currentEventIndex < events.size() - 1) {
        currentEventIndex++;
        displayCurrentEvent();
    }
}

void ReplayWindow::onPrevEventClicked() {
    if (currentEventIndex > 0) {
        currentEventIndex--;
        displayCurrentEvent();
    }
}

void ReplayWindow::playReplay() {
    isPlaying = true;
    ui->btnPlay->setEnabled(false);
    ui->btnPause->setEnabled(true);
    playbackTimer->start(500);
}

void ReplayWindow::pauseReplay() {
    isPlaying = false;
    playbackTimer->stop();
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
}

void ReplayWindow::stopReplay() {
    isPlaying = false;
    playbackTimer->stop();
    currentEventIndex = 0;
    displayCurrentEvent();
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
}

void ReplayWindow::onReplayTick() {
    if (isPlaying && currentEventIndex < events.size() - 1) {
        currentEventIndex++;
        displayCurrentEvent();
    } else if (currentEventIndex >= events.size() - 1) {
        pauseReplay();
    }
}

void ReplayWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}

```

---

##  ui/GameWindow.h 

```cpp
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QStringList>
#include "protocol.h"
#include "../models/GameState.h"

namespace Ui {
class GameWindow;
}

class NetworkManager;
class SessionState;

class GameWindow : public QMainWindow {
    Q_OBJECT
public:
    GameWindow(GameMode mode, uint32_t roomId, uint32_t hostUserId,
               const QVector<PlayerInfo>& players, QWidget* parent = nullptr);
    ~GameWindow();

private slots:
    void onQuestionReceived(uint32_t questionId, const QString& content,
                            const QStringList& options, uint32_t timeLimitSec);
    void onRoundResult(uint8_t correctOption, uint8_t playerCount,
                       const QVector<PlayerRoundResult>& results);
    void onPlayerEliminated(uint32_t userId);
    void onGameOver(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings, 
                   uint8_t gameEndReason, uint8_t winnerCount);
    void onGamePaused();
    void onGameResumed();
    void onGameTerminated(TerminationReason reason);
    void onConnectionError(const QString& error);

    void onOptionAClicked();
    void onOptionBClicked();
    void onOptionCClicked();
    void onOptionDClicked();
    void onQuestionTick();
    void onReturnedToRoom();
    void onPauseGameClicked();
    void onResumeGameClicked();

private:
    void setupUiTheme();
    void bindSignals();
    void startQuestion(uint32_t questionId, const QString& content,
                       const QStringList& options, uint32_t timeLimitSec);
    void setButtonsEnabled(bool enabled);
    void markSelection(int optionIndex);
    void highlightCorrect(uint8_t correctOption);
    void resetOptionStyles();
    void startQuestionTimer(uint32_t seconds);
    void stopQuestionTimer();
    void updateTimerUi();
    void updateScoreboard();
    QString formatStatus(const ClientPlayer& p) const;
    void applyRoundResults(const QVector<PlayerRoundResult>& results);
    void setFeedback(const QString& text, bool warn = false);
    void handleAnswerSelection(int optionIndex);

    Ui::GameWindow* ui;
    NetworkManager* networkManager;
    SessionState* sessionState;

    GameMode gameMode;
    uint32_t roomId;
    uint32_t hostUserId;

    QVector<ClientPlayer> players;

    QTimer* questionTimer;
    QElapsedTimer answerElapsed;
    int timeRemainingSec;
    uint32_t currentQuestionId;
    uint8_t questionNumber;
    bool questionActive;
    bool answerSent;
    bool paused;

signals:
    void returnedToRoom();
};

```

---

##  ui/ReplayWindow.h 

```cpp
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QMap>
#include "protocol.h"
#include "../models/GameState.h"

namespace Ui {
class ReplayWindow;
}

class NetworkManager;

class ReplayWindow : public QMainWindow {
    Q_OBJECT

public:
    ReplayWindow(uint32_t sessionId, QWidget* parent = nullptr);
    ~ReplayWindow();

private slots:
    void onReplayDataReceived(StatusCode status, uint32_t sessionId, GameMode mode, 
                             const QVector<ReplayEvent>& events);
    void onReplayTick();
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onNextEventClicked();
    void onPrevEventClicked();
    void onConnectionError(const QString& error);

private:
    void setupUi();
    void loadReplayData();
    void playReplay();
    void pauseReplay();
    void stopReplay();
    void displayCurrentEvent();
    void updateEventList();
    void buildEventLog();

    Ui::ReplayWindow* ui;
    NetworkManager* networkManager;

    uint32_t sessionId;
    GameMode gameMode;
    QVector<ReplayEvent> events;
    
    int currentEventIndex;
    bool isPlaying;
    QTimer* playbackTimer;
    
    struct QuestionData {
        uint32_t questionId;
        QStringList options;
        QMap<uint32_t, uint8_t> playerAnswers;
        QMap<uint32_t, uint32_t> playerResponseTimes;
    };
    
    QMap<uint32_t, QuestionData> questions;
};

```

---

##  ui/RoomWindow.cpp 

```cpp
#include "RoomWindow.h"
#include "ui_RoomWindow.h"
#include "GameWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"
#include <algorithm>
#include <QMessageBox>
#include <QTableWidgetItem>

RoomWindow::RoomWindow(const RoomInfo& roomInfo, uint32_t hostUserId, 
                       const QVector<PlayerInfo>& players, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::RoomWindow), currentRoom(roomInfo), 
      hostUserId(hostUserId), cachedPlayers(players)
{
    ui->setupUi(this);

    networkManager = &NetworkManager::instance();
    sessionState = &SessionState::instance();

    // Check if local player is host
    isLocalPlayerHost = (hostUserId == sessionState->getUserId());

    setupUI();
    setupConnections();
    setupAutoRefresh();

    // Initial population
    populatePlayerTable(cachedPlayers);
}

RoomWindow::~RoomWindow()
{
    stopAutoRefresh();
    stopCountdownTimer();
    delete ui;
}

void RoomWindow::show()
{
    QMainWindow::show();
}

void RoomWindow::setupUI()
{
    // Update header with room info
    // Initial header is based on current cached players; will be updated on join/leave events
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name, sizeof(currentRoom.room_name)))
        .arg(cachedPlayers.size())
        .arg(currentRoom.max_players)
        .arg(modeStr);
    ui->lblRoomInfo->setText(headerText);

    // Configure player table
    ui->tblPlayers->setColumnCount(3);
    ui->tblPlayers->setHorizontalHeaderLabels({"Player Name", "Status", "Role"});
    ui->tblPlayers->setColumnWidth(0, 250);
    ui->tblPlayers->setColumnWidth(1, 150);
    ui->tblPlayers->setColumnWidth(2, 150);
    ui->tblPlayers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPlayers->setSelectionMode(QAbstractItemView::SingleSelection);

    // Hide start game button if not host
    if (!isLocalPlayerHost) {
        ui->btnStartGame->hide();
    } else {
        ui->btnStartGame->show();
        disableStartGameButton();
    }
}

void RoomWindow::setupConnections()
{
    // Button connections
    connect(ui->btnToggleReady, &QPushButton::clicked, this, &RoomWindow::onToggleReadyClicked);
    connect(ui->btnStartGame, &QPushButton::clicked, this, &RoomWindow::onStartGameClicked);
    connect(ui->btnLeaveRoom, &QPushButton::clicked, this, &RoomWindow::onLeaveRoomClicked);

    // NetworkManager signal connections (using existing signals)
    // For ready toggle, we'll use the existing readyStatusNotif signal
    connect(networkManager, &NetworkManager::readyStatusNotif,
            this, [this](uint32_t userId, bool ready) {
                if (userId == sessionState->getUserId()) {
                    isLocalPlayerReady = ready;
                    ui->btnToggleReady->setText(ready ? "Mark Unready" : "Mark Ready");
                }
                onPlayerReadyChanged(userId, ready);
            });
    
    // Handle join room response with full player list
    connect(networkManager, &NetworkManager::joinRoomResponse,
            this, [this](StatusCode code, const RoomInfo& /*info*/, uint8_t playerCount, 
                        const QVector<PlayerInfo>& players, uint32_t hostId) {
                if (code == StatusCode::SUCCESS) {
                    hostUserId = hostId;
                    onPlayerListUpdate(playerCount, players);
                }
            });
    
    // For game start notifications
    connect(networkManager, &NetworkManager::gameStartNotif,
            this, &RoomWindow::onGameStarted);

    // Fallback: if a question arrives but we are still in the room view, transition to game
    connect(networkManager, &NetworkManager::questionNotif,
            this, [this](uint32_t /*qid*/, const QString& /*content*/, const QStringList& /*opts*/, uint32_t /*tl*/) {
                if (!gameWindow) {
                    onGameStarted();
                }
            });
    
    // For player notifications
    connect(networkManager, &NetworkManager::playerJoinedNotif,
            this, &RoomWindow::onPlayerJoined);
    connect(networkManager, &NetworkManager::playerLeftNotif,
            this, &RoomWindow::onPlayerLeft);
    
    // Network error
    connect(networkManager, &NetworkManager::connectionError,
            this, &RoomWindow::onNetworkError);
}

void RoomWindow::setupAutoRefresh()
{
    if (!refreshTimer) {
        refreshTimer = new QTimer(this);
        refreshTimer->setInterval(2000); // 2 seconds
        connect(refreshTimer, &QTimer::timeout, this, &RoomWindow::onRefreshPlayerList);
    }
    refreshTimer->start();
}

void RoomWindow::stopAutoRefresh()
{
    if (refreshTimer) {
        refreshTimer->stop();
    }
}

void RoomWindow::onToggleReadyClicked()
{
    // Send ready toggle request to server
    isLocalPlayerReady = !isLocalPlayerReady;
    networkManager->sendReadyStatus(isLocalPlayerReady);
    
    // Update button text
    ui->btnToggleReady->setText(isLocalPlayerReady ? "Mark Unready" : "Mark Ready");

    // Optimistically update local table for immediate feedback
    for (int i = 0; i < cachedPlayers.size(); ++i) {
        if (cachedPlayers[i].user_id == sessionState->getUserId()) {
            cachedPlayers[i].is_ready = isLocalPlayerReady;
            break;
        }
    }
    populatePlayerTable(cachedPlayers);
}

void RoomWindow::onStartGameClicked()
{
    if (!isLocalPlayerHost) {
        QMessageBox::warning(this, "Error", "Only the host can start the game");
        return;
    }

    if (!allPlayersReady()) {
        QMessageBox::information(this, "Not Ready", 
            "All players must be ready before starting the game");
        return;
    }

    // Send start game request to server
    networkManager->sendStartGame();
    
    // Disable button and show waiting status
    ui->btnStartGame->setEnabled(false);
    ui->lblGameCountdown->setText("Starting game...");
}

void RoomWindow::onLeaveRoomClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Leave Room", "Are you sure you want to leave this room?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        networkManager->sendLeaveRoom();
        this->close();
    }
}

void RoomWindow::onGameStartCountdown(uint8_t secondsRemaining)
{
    startCountdownTimer(secondsRemaining);
}

void RoomWindow::onGameStarted()
{
    stopAutoRefresh();
    stopCountdownTimer();

    // Launch GameWindow with current players and room info
    if (gameWindow) {
        gameWindow->close();
        gameWindow = nullptr;
    }

    gameWindow = new GameWindow(currentRoom.game_mode, currentRoom.room_id, hostUserId, cachedPlayers, this);
    
    // Connect return-to-room signal
    connect(gameWindow, &GameWindow::returnedToRoom, this, &RoomWindow::onReturnedToRoom);
    
    this->hide();
    gameWindow->show();
}

void RoomWindow::onPlayerListUpdate(uint8_t playerCount, const QVector<PlayerInfo>& players)
{
    cachedPlayers = players;
    populatePlayerTable(players);

    // Update header
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name, sizeof(currentRoom.room_name)))
        .arg(cachedPlayers.size())
        .arg(currentRoom.max_players)
        .arg(modeStr);
    ui->lblRoomInfo->setText(headerText);

    // Check if all players ready
    if (isLocalPlayerHost && allPlayersReady()) {
        enableStartGameButton();
    } else if (isLocalPlayerHost) {
        disableStartGameButton();
    }
}

void RoomWindow::onPlayerJoined(const PlayerInfo& player)
{
    // Update cache and rebuild table to keep indices consistent across clients
    cachedPlayers.append(player);
    populatePlayerTable(cachedPlayers);

    // Update header player count
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name, sizeof(currentRoom.room_name)))
        .arg(cachedPlayers.size())
        .arg(currentRoom.max_players)
        .arg(modeStr);
    ui->lblRoomInfo->setText(headerText);

    // Host button state may change depending on readiness
    if (isLocalPlayerHost) {
        if (allPlayersReady()) enableStartGameButton(); else disableStartGameButton();
    }
}

void RoomWindow::onPlayerLeft(uint32_t userId)
{
    // Remove from cached players by userId and repopulate table
    cachedPlayers.erase(
        std::remove_if(cachedPlayers.begin(), cachedPlayers.end(),
            [userId](const PlayerInfo& p) { return p.user_id == userId; }),
        cachedPlayers.end()
    );
    populatePlayerTable(cachedPlayers);

    // Update header player count
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name, sizeof(currentRoom.room_name)))
        .arg(cachedPlayers.size())
        .arg(currentRoom.max_players)
        .arg(modeStr);
    ui->lblRoomInfo->setText(headerText);
}

void RoomWindow::onPlayerReadyChanged(uint32_t userId, bool isReady)
{
    // Update cached players and corresponding table row only for the target user
    int targetRow = -1;
    for (int i = 0; i < cachedPlayers.size(); ++i) {
        if (cachedPlayers[i].user_id == userId) {
            cachedPlayers[i].is_ready = isReady;
            targetRow = i;
            break;
        }
    }

    // Rebuild table to avoid row index mismatches across clients
    populatePlayerTable(cachedPlayers);

    // Check if all players ready
    if (isLocalPlayerHost && allPlayersReady()) {
        enableStartGameButton();
    } else if (isLocalPlayerHost) {
        disableStartGameButton();
    }
}

void RoomWindow::onNetworkError(const QString& error)
{
    ui->lblError->setText(error);
}

void RoomWindow::onCountdownTick()
{
    if (countdownSecondsRemaining > 0) {
        countdownSecondsRemaining--;
        ui->lblGameCountdown->setText(
            QString("Game starting in %1...").arg(countdownSecondsRemaining));
    } else {
        stopCountdownTimer();
        ui->lblGameCountdown->setText("Game started!");
    }
}

void RoomWindow::onRefreshPlayerList()
{
    // In a real implementation, request updated player list from server
    // For now, player updates come through notifications (playerJoined, playerLeft, playerReadyChanged)
}

void RoomWindow::populatePlayerTable(const QVector<PlayerInfo>& players)
{
    ui->tblPlayers->setRowCount(0);
    uint32_t localUserId = sessionState->getUserId();

    for (const auto& player : players) {
        int rowCount = ui->tblPlayers->rowCount();
        ui->tblPlayers->insertRow(rowCount);

        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromLatin1(player.display_name, sizeof(player.display_name)));
        QTableWidgetItem *statusItem = new QTableWidgetItem(formatPlayerStatus(player.is_ready, true));
        QTableWidgetItem *roleItem = new QTableWidgetItem(formatPlayerRole(player.user_id));

        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        roleItem->setFlags(roleItem->flags() & ~Qt::ItemIsEditable);

        // Bold ONLY for local player
        if (player.user_id == localUserId) {
            QFont boldFont = nameItem->font();
            boldFont.setBold(true);
            nameItem->setFont(boldFont);
            statusItem->setFont(boldFont);
            roleItem->setFont(boldFont);
        }

        ui->tblPlayers->setItem(rowCount, 0, nameItem);
        ui->tblPlayers->setItem(rowCount, 1, statusItem);
        ui->tblPlayers->setItem(rowCount, 2, roleItem);
    }
}

QString RoomWindow::formatPlayerStatus(bool isReady, bool isConnected) const
{
    if (!isConnected) return "Disconnected";
    return isReady ? "Ready ✓" : "Not Ready";
}

QString RoomWindow::formatPlayerRole(uint32_t userId) const
{
    if (userId == hostUserId) {
        return "Host";
    } else if (userId == sessionState->getUserId()) {
        return "You";
    } else {
        return "Player";
    }
}

void RoomWindow::enableStartGameButton()
{
    ui->btnStartGame->setEnabled(true);
    ui->lblError->setText("");
}

void RoomWindow::disableStartGameButton()
{
    ui->btnStartGame->setEnabled(false);
}

bool RoomWindow::allPlayersReady() const
{
    for (const auto& player : cachedPlayers) {
        if (!player.is_ready) {
            return false;
        }
    }
    return true;
}

void RoomWindow::startCountdownTimer(uint8_t seconds)
{
    countdownSecondsRemaining = seconds;

    if (!countdownTimer) {
        countdownTimer = new QTimer(this);
        connect(countdownTimer, &QTimer::timeout, this, &RoomWindow::onCountdownTick);
    }

    countdownTimer->setInterval(1000); // 1 second
    countdownTimer->start();

    ui->lblGameCountdown->setText(
        QString("Game starting in %1...").arg(countdownSecondsRemaining));
}

void RoomWindow::stopCountdownTimer()
{
    if (countdownTimer) {
        countdownTimer->stop();
    }
    countdownSecondsRemaining = 0;
}

void RoomWindow::onReturnedToRoom()
{
    // Close game window and show room window again
    if (gameWindow) {
        gameWindow->close();
        gameWindow = nullptr;
    }
    
    // Reset local player ready state
    isLocalPlayerReady = false;
    ui->btnToggleReady->setText("Mark Ready");
    
    // Show this window
    this->show();
    this->raise();
    this->activateWindow();
    
    // Restart auto-refresh to sync player list
    setupAutoRefresh();
}

```

---

##  ui/GameHistoryWindow.cpp 

```cpp
#include "GameHistoryWindow.h"
#include "ui_GameHistoryWindow.h"
#include "../network/NetworkManager.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>

GameHistoryWindow::GameHistoryWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameHistoryWindow)
    , networkManager(&NetworkManager::instance())
{
    ui->setupUi(this);
    setWindowTitle("Game History");
    
    connect(networkManager, &NetworkManager::gameHistoryResponse,
            this, &GameHistoryWindow::onGameHistoryReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &GameHistoryWindow::onConnectionError);
    
    ui->tblHistory->setColumnCount(7);
    ui->tblHistory->setHorizontalHeaderLabels({
        "Date", "Mode", "Score", "Rank", "Correct", "Total", "Avg Time (ms)"
    });
    ui->tblHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    loadHistory();
}

GameHistoryWindow::~GameHistoryWindow() {
    delete ui;
}

void GameHistoryWindow::loadHistory() {
    ui->lblStatus->setText("Loading game history...");
    ui->tblHistory->setRowCount(0);
    networkManager->sendGetGameHistory();
}

void GameHistoryWindow::onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries) {
    if (status == StatusCode::SUCCESS) {
        displayHistory(entries);
    } else {
        ui->lblStatus->setText("Failed to load game history");
    }
}

void GameHistoryWindow::displayHistory(const QVector<GameHistoryEntry>& entries) {
    ui->tblHistory->setRowCount(0);
    
    for (const auto& entry : entries) {
        int row = ui->tblHistory->rowCount();
        ui->tblHistory->insertRow(row);
        
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(entry.timestamp_sec);
        QString dateStr = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        
        auto* dateItem = new QTableWidgetItem(dateStr);
        auto* modeItem = new QTableWidgetItem(QString::fromLatin1(entry.game_mode));
        auto* scoreItem = new QTableWidgetItem(QString::number(entry.player_score));
        auto* rankItem = new QTableWidgetItem(entry.player_rank > 0 ? QString::number(entry.player_rank) : "—");
        auto* correctItem = new QTableWidgetItem(QString("%1/%2").arg(entry.correct_answers).arg(entry.total_questions));
        auto* totalItem = new QTableWidgetItem(QString::number(entry.total_questions));
        auto* timeItem = new QTableWidgetItem(QString::number(entry.avg_response_time_ms));
        
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        modeItem->setFlags(modeItem->flags() & ~Qt::ItemIsEditable);
        scoreItem->setFlags(scoreItem->flags() & ~Qt::ItemIsEditable);
        rankItem->setFlags(rankItem->flags() & ~Qt::ItemIsEditable);
        correctItem->setFlags(correctItem->flags() & ~Qt::ItemIsEditable);
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        
        ui->tblHistory->setItem(row, 0, dateItem);
        ui->tblHistory->setItem(row, 1, modeItem);
        ui->tblHistory->setItem(row, 2, scoreItem);
        ui->tblHistory->setItem(row, 3, rankItem);
        ui->tblHistory->setItem(row, 4, correctItem);
        ui->tblHistory->setItem(row, 5, totalItem);
        ui->tblHistory->setItem(row, 6, timeItem);
    }
    
    ui->lblStatus->setText(QString("Showing %1 games").arg(entries.size()));
}

void GameHistoryWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}

```

---

##  ui/MainMenuWindow.cpp 

```cpp
#include "MainMenuWindow.h"
#include "ui_MainMenuWindow.h"
#include "LobbyWindow.h"
#include "ReplayWindow.h"
#include "GameHistoryWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

MainMenuWindow::MainMenuWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainMenuWindow)
    , networkManager(&NetworkManager::instance())
{
    ui->setupUi(this);
    setWindowTitle("Quiz Game - Main Menu");
    
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainMenuWindow::onPlayClicked);
    connect(ui->btnViewStats, &QPushButton::clicked, this, &MainMenuWindow::onViewStatsClicked);
    connect(ui->btnReplay, &QPushButton::clicked, this, &MainMenuWindow::onReplayClicked);
    connect(ui->btnGameHistory, &QPushButton::clicked, this, &MainMenuWindow::onGameHistoryClicked);
    
    connect(networkManager, &NetworkManager::statsResponse,
            this, &MainMenuWindow::onStatsReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &MainMenuWindow::onConnectionError);
    
    ui->btnPlay->setStyleSheet("background:#27ae60; color:white; font-weight:bold; font-size:14px;");
    ui->btnViewStats->setStyleSheet("background:#3498db; color:white; font-weight:bold; font-size:14px;");
    ui->btnReplay->setStyleSheet("background:#9b59b6; color:white; font-weight:bold; font-size:14px;");
    ui->btnGameHistory->setStyleSheet("background:#e67e22; color:white; font-weight:bold; font-size:14px;");
    
    loadStats();
}

MainMenuWindow::~MainMenuWindow() {
    delete ui;
}

void MainMenuWindow::loadStats() {
    ui->lblLoading->setText("Loading stats...");
    ui->btnPlay->setEnabled(false);
    ui->btnViewStats->setEnabled(false);
    ui->btnReplay->setEnabled(false);
    
    networkManager->sendGetStats();
}

void MainMenuWindow::onStatsReceived(const UserStatsResponse& stats) {
    currentStats = stats;
    displayStats(stats);
    
    ui->btnPlay->setEnabled(true);
    ui->btnViewStats->setEnabled(true);
    ui->btnReplay->setEnabled(true);
    ui->lblLoading->clear();
}

void MainMenuWindow::displayStats(const UserStatsResponse& stats) {
    double percentile = 0.0;
    if (stats.total_ranked_players > 0) {
        percentile = (100.0 * (stats.total_ranked_players - stats.player_rank)) / stats.total_ranked_players;
    }
    
    QString tierName = getTierName(stats.ranked_points);
    QString rankText = QString("%1, %2th percentile").arg(stats.player_rank).arg(percentile, 0, 'f', 1);
    
    QString statsText = QString(
        "<b>%1</b><br>"
        "<b>%2</b> - %3<br><br>"
        "<b>Ranked Points:</b> %4<br><br>"
        "<b>Elimination Mode:</b><br>"
        "Matches: %5 | Wins: %6 | High Score: %7<br>"
        "Accuracy: %8%<br><br>"
        "<b>Scoring Mode:</b><br>"
        "Matches: %9 | Wins: %10 | High Score: %11<br>"
        "Accuracy: %12%"
    ).arg(SessionState::instance().getDisplayName())
     .arg(tierName)
     .arg(rankText)
     .arg(stats.ranked_points)
     .arg(stats.elimination.total_matches)
     .arg(stats.elimination.wins)
     .arg(stats.elimination.high_score)
     .arg(stats.elimination.total_matches > 0 ? 
          (100.0 * stats.elimination.total_correct_answers / 
           (stats.elimination.total_correct_answers + stats.elimination.total_incorrect_answers)) : 0.0, 0, 'f', 1)
     .arg(stats.scoring.total_matches)
     .arg(stats.scoring.wins)
     .arg(stats.scoring.high_score)
     .arg(stats.scoring.total_matches > 0 ? 
          (100.0 * stats.scoring.total_correct_answers / 
           (stats.scoring.total_correct_answers + stats.scoring.total_incorrect_answers)) : 0.0, 0, 'f', 1);
    
    ui->lblStats->setText(statsText);
}

void MainMenuWindow::onPlayClicked() {
    LobbyWindow* lobbyWindow = new LobbyWindow(this);
    lobbyWindow->show();
    this->hide();
}

void MainMenuWindow::onViewStatsClicked() {
    QString msg = QString(
        "Player: %1\n"
        "Ranked Points: %2\n\n"
        "Elimination Mode:\n"
        "Matches: %3\nWins: %4\nHigh Score: %5\n\n"
        "Scoring Mode:\n"
        "Matches: %6\nWins: %7\nHigh Score: %8"
    ).arg(SessionState::instance().getDisplayName())
     .arg(currentStats.ranked_points)
     .arg(currentStats.elimination.total_matches)
     .arg(currentStats.elimination.wins)
     .arg(currentStats.elimination.high_score)
     .arg(currentStats.scoring.total_matches)
     .arg(currentStats.scoring.wins)
     .arg(currentStats.scoring.high_score);
    
    QMessageBox::information(this, "Your Stats", msg);
}

void MainMenuWindow::onReplayClicked() {
    bool ok;
    uint32_t sessionId = QInputDialog::getInt(
        this, "Replay Game", 
        "Enter session ID to replay:", 
        1, 1, 2147483647, 1, &ok);
    
    if (ok) {
        ReplayWindow* replayWindow = new ReplayWindow(sessionId, this);
        replayWindow->show();
    }
}

void MainMenuWindow::onGameHistoryClicked() {
    GameHistoryWindow* historyWindow = new GameHistoryWindow(this);
    historyWindow->show();
}

void MainMenuWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}

QString MainMenuWindow::getTierName(uint32_t rankedPoints) const {
    if (rankedPoints >= 2400) return "Diamond";
    if (rankedPoints >= 2000) return "Platinum";
    if (rankedPoints >= 1600) return "Gold";
    if (rankedPoints >= 1200) return "Silver";
    return "Bronze";
}

```

---

##  ui/LoginWindow.cpp 

```cpp
#include "LoginWindow.h"
#include "MainMenuWindow.h"
#include "ui_LoginWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"
#include <QMessageBox>
#include <QRegularExpression>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , mainMenuWindow(nullptr)
{
    ui->setupUi(this);
    
    // Connect UI signals
    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    
    // Connect network signals
    auto& network = NetworkManager::instance();
    connect(&network, &NetworkManager::connected, this, &LoginWindow::onConnected);
    connect(&network, &NetworkManager::disconnected, this, &LoginWindow::onDisconnected);
    connect(&network, &NetworkManager::connectionError, this, &LoginWindow::onConnectionError);
    connect(&network, &NetworkManager::registerResponse, this, &LoginWindow::onRegisterResponse);
    connect(&network, &NetworkManager::loginResponse, this, &LoginWindow::onLoginResponse);
    
    // Connect to server on startup
    connectToServer();
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::connectToServer() {
    auto& network = NetworkManager::instance();
    
    // TODO: Make this configurable via settings dialog
    QString host = "127.0.0.1";
    quint16 port = 10002;
    
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
    ui->lblError->setText("Connecting to server...");
    
    if (!network.connectToServer(host, port)) {
        showError("Failed to connect to server!");
        ui->btnLogin->setEnabled(true);
        ui->btnRegister->setEnabled(true);
    }
}

void LoginWindow::onLoginClicked() {
    if (!validateLoginInput()) {
        return;
    }
    
    QString email = ui->txtLoginEmail->text().trimmed();
    QString password = ui->txtLoginPassword->text();
    
    ui->btnLogin->setEnabled(false);
    clearError();
    
    NetworkManager::instance().sendLogin(email, password);
}

void LoginWindow::onRegisterClicked() {
    if (!validateRegisterInput()) {
        return;
    }
    
    QString email = ui->txtRegEmail->text().trimmed();
    QString displayName = ui->txtRegDisplayName->text().trimmed();
    QString password = ui->txtRegPassword->text();
    
    ui->btnRegister->setEnabled(false);
    clearError();
    
    NetworkManager::instance().sendRegister(email, displayName, password);
}

bool LoginWindow::validateLoginInput() {
    QString email = ui->txtLoginEmail->text().trimmed();
    QString password = ui->txtLoginPassword->text();
    
    if (email.isEmpty()) {
        showError("Please enter your email");
        return false;
    }
    
    if (password.isEmpty()) {
        showError("Please enter your password");
        return false;
    }
    
    return true;
}

bool LoginWindow::validateRegisterInput() {
    QString email = ui->txtRegEmail->text().trimmed();
    QString displayName = ui->txtRegDisplayName->text().trimmed();
    QString password = ui->txtRegPassword->text();
    QString confirmPassword = ui->txtRegConfirmPassword->text();
    
    // Validate email
    if (email.isEmpty()) {
        showError("Please enter your email");
        return false;
    }
    
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        showError("Please enter a valid email address");
        return false;
    }
    
    // Validate display name
    if (displayName.isEmpty()) {
        showError("Please enter a display name");
        return false;
    }
    
    if (displayName.length() > 32) {
        showError("Display name must be 32 characters or less");
        return false;
    }
    
    // Validate password
    if (password.isEmpty()) {
        showError("Please enter a password");
        return false;
    }
    
    if (password.length() < 6) {
        showError("Password must be at least 6 characters");
        return false;
    }
    
    if (password != confirmPassword) {
        showError("Passwords do not match");
        return false;
    }
    
    return true;
}

void LoginWindow::onConnected() {
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
    clearError();
}

void LoginWindow::onDisconnected() {
    showError("Disconnected from server");
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
}

void LoginWindow::onConnectionError(const QString& error) {
    showError("Connection error: " + error);
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
}

void LoginWindow::onRegisterResponse(StatusCode code) {
    ui->btnRegister->setEnabled(true);
    
    if (code == StatusCode::SUCCESS) {
        QMessageBox::information(this, "Success", "Registration successful! Please login.");
        ui->tabWidget->setCurrentIndex(0); // Switch to login tab
        clearError();
    } else if (code == StatusCode::EMAIL_EXISTS) {
        showError("Email already exists!");
    } else {
        showError("Registration failed. Please try again.");
    }
}

void LoginWindow::onLoginResponse(StatusCode code, uint32_t userId, const QString& displayName) {
    ui->btnLogin->setEnabled(true);
    
    if (code == StatusCode::SUCCESS) {
        // Store session state
        SessionState::instance().setAuthenticated(true);
        SessionState::instance().setUserId(userId);
        SessionState::instance().setDisplayName(displayName);
        SessionState::instance().setEmail(ui->txtLoginEmail->text().trimmed());

        clearError();
        
        // Hide LoginWindow and show MainMenuWindow
        this->hide();
        
        // Create and show MainMenuWindow
        if (!mainMenuWindow) {
            mainMenuWindow = new MainMenuWindow();
        }
        mainMenuWindow->show();
    } else if (code == StatusCode::INVALID_CREDENTIALS) {
        showError("Invalid email or password!");
    } else {
        showError("Login failed. Please try again.");
    }
}

void LoginWindow::showError(const QString& message) {
    ui->lblError->setText(message);
}

void LoginWindow::clearError() {
    ui->lblError->setText("");
}

```

---

##  ui/GameHistoryWindow.h 

```cpp
#pragma once

#include <QMainWindow>
#include <QVector>
#include "protocol.h"

namespace Ui {
class GameHistoryWindow;
}

class NetworkManager;

class GameHistoryWindow : public QMainWindow {
    Q_OBJECT

public:
    GameHistoryWindow(QWidget* parent = nullptr);
    ~GameHistoryWindow();

private slots:
    void onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries);
    void onConnectionError(const QString& error);

private:
    void loadHistory();
    void displayHistory(const QVector<GameHistoryEntry>& entries);

    Ui::GameHistoryWindow* ui;
    NetworkManager* networkManager;
};

```

---

##  ui/RoomWindow.h 

```cpp
#ifndef ROOMWINDOW_H
#define ROOMWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "protocol.h"

namespace Ui {
class RoomWindow;
}

class NetworkManager;
class SessionState;
class GameWindow;

class RoomWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit RoomWindow(const RoomInfo& roomInfo, uint32_t hostUserId, 
                       const QVector<PlayerInfo>& players, QWidget *parent = nullptr);
    ~RoomWindow();

    void show();

private slots:
    // Button slots
    void onToggleReadyClicked();
    void onStartGameClicked();
    void onLeaveRoomClicked();

    // Network response slots
    void onGameStartCountdown(uint8_t secondsRemaining);
    void onGameStarted();
    void onPlayerListUpdate(uint8_t playerCount, const QVector<PlayerInfo>& players);
    void onPlayerJoined(const PlayerInfo& player);
    void onPlayerLeft(uint32_t userId);
    void onPlayerReadyChanged(uint32_t userId, bool isReady);
    void onReturnedToRoom();

    // Network error
    void onNetworkError(const QString& error);

    // Countdown timer
    void onCountdownTick();

    // Auto-refresh for player list
    void onRefreshPlayerList();

private:
    void setupUI();
    void setupConnections();
    void setupAutoRefresh();
    void stopAutoRefresh();

    void populatePlayerTable(const QVector<PlayerInfo>& players);
    QString formatPlayerStatus(bool isReady, bool isConnected) const;
    QString formatPlayerRole(uint32_t userId) const;

    void enableStartGameButton();
    void disableStartGameButton();
    bool allPlayersReady() const;

    void startCountdownTimer(uint8_t seconds);
    void stopCountdownTimer();

    Ui::RoomWindow *ui;
    NetworkManager *networkManager;
    SessionState *sessionState;

    RoomInfo currentRoom;
    uint32_t hostUserId;
    bool isLocalPlayerReady = false;
    bool isLocalPlayerHost = false;

    QTimer *refreshTimer = nullptr;           // Auto-refresh player list
    QTimer *countdownTimer = nullptr;         // Game start countdown
    uint8_t countdownSecondsRemaining = 0;
    QVector<PlayerInfo> cachedPlayers;
    GameWindow *gameWindow = nullptr;
};

#endif // ROOMWINDOW_H

```

---

