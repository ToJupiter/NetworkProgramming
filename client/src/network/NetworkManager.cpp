#include "NetworkManager.h"
#include "ProtocolHelper.h"
#include <QDebug>

NetworkManager& NetworkManager::instance() {
    static NetworkManager inst;
    return inst;
}

NetworkManager::NetworkManager()
    : QObject(nullptr)
    , m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,
            this, &NetworkManager::onSocketError);
}

NetworkManager::~NetworkManager() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool NetworkManager::connectToServer(const QString& host, quint16 port) {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }
    
    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected(5000);
}

void NetworkManager::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool NetworkManager::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::sendMessage(MessageType type, const QByteArray& body) {
    if (!isConnected()) {
        qWarning() << "Cannot send message: not connected";
        return;
    }
    
    QByteArray message = ProtocolHelper::createMessage(type, body);
    m_socket->write(message);
    m_socket->flush();
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

void NetworkManager::sendSubmitAnswer(uint32_t questionId, uint8_t selectedOption, uint32_t responseTimeMs) {
    SubmitAnswerRequest req;
    req.question_id = questionId;
    req.selected_option = selectedOption;
    req.response_time_ms = responseTimeMs;
    
    sendMessage(MessageType::C2S_SUBMIT_ANSWER_REQ, ProtocolHelper::packStruct(req));
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

// === Socket Event Handlers ===

void NetworkManager::onConnected() {
    qDebug() << "Connected to server";
    emit connected();
}

void NetworkManager::onDisconnected() {
    qDebug() << "Disconnected from server";
    m_recvBuffer.clear();
    emit disconnected();
}

void NetworkManager::onReadyRead() {
    m_recvBuffer.append(m_socket->readAll());
    processBuffer();
}

void NetworkManager::onSocketError(QAbstractSocket::SocketError error) {
    QString errorMsg = m_socket->errorString();
    qWarning() << "Socket error:" << error << errorMsg;
    emit connectionError(errorMsg);
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
            emit gameOverNotif(notif.result_count, rankings);
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
        
        case MessageType::S2C_GET_STATS_RSP: {
            auto resp = ProtocolHelper::unpackStruct<UserStatsResponse>(body);
            emit statsResponse(resp);
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
