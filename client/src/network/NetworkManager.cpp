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
