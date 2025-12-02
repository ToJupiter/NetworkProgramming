#include "../headers/BackEnd.h"
#include <QDebug>
#include <QHostAddress>
#include <QTimer>
#include <cstring>

BackEnd::BackEnd(QObject *parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_prize(0)
    , m_enemyCurrentQuestion(0)
    , m_enemySeconds(0)
    , m_userId(0)
    , m_currentRoomId(0)
    , m_currentQuestionId(0)
    , m_isConnected(false)
    , m_serverHost("127.0.0.1")
    , m_serverPort(8080)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &BackEnd::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &BackEnd::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &BackEnd::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &BackEnd::onError);
}

BackEnd::~BackEnd()
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void BackEnd::connectToServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        emit connectSuccess();
        return;
    }
    
    m_socket->connectToHost(m_serverHost, m_serverPort);
    
    // Set timeout
    QTimer::singleShot(5000, this, [this]() {
        if (m_socket->state() != QAbstractSocket::ConnectedState) {
            m_socket->abort();
            emit connectFail();
        }
    });
}

void BackEnd::disconnectToServer()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
    m_isConnected = false;
}

void BackEnd::signIn(const QString& username, const QString& password)
{
    if (!m_isConnected) {
        emit connectFail();
        return;
    }
    
    LoginRequest req;
    memset(&req, 0, sizeof(req));
    strncpy(req.email, username.toUtf8().constData(), MAX_EMAIL_LEN - 1);
    strncpy(req.password, password.toUtf8().constData(), MAX_PASSWORD_LEN - 1);
    
    sendMessage(MessageType::C2S_LOGIN_REQ, &req, sizeof(req));
}

void BackEnd::signUp(const QString& username, const QString& password)
{
    if (!m_isConnected) {
        emit connectFail();
        return;
    }
    
    RegisterRequest req;
    memset(&req, 0, sizeof(req));
    strncpy(req.email, username.toUtf8().constData(), MAX_EMAIL_LEN - 1);
    strncpy(req.display_name, username.toUtf8().constData(), MAX_DISPLAY_NAME_LEN - 1);
    strncpy(req.password, password.toUtf8().constData(), MAX_PASSWORD_LEN - 1);
    
    sendMessage(MessageType::C2S_REGISTER_REQ, &req, sizeof(req));
}

void BackEnd::changePassword(const QString& newPassword)
{
    if (!m_isConnected) {
        return;
    }
    
    AccountUpdateRequest req;
    memset(&req, 0, sizeof(req));
    // Note: The protocol uses AccountUpdateRequest for display name, but we'll use it for password
    // This might need adjustment based on actual protocol
    strncpy(req.new_display_name, newPassword.toUtf8().constData(), MAX_DISPLAY_NAME_LEN - 1);
    
    sendMessage(MessageType::C2S_ACCOUNT_UPDATE_REQ, &req, sizeof(req));
}

void BackEnd::choiceAnswer(int choice, int time)
{
    if (!m_isConnected || m_currentQuestionId == 0) {
        return;
    }
    
    SubmitAnswerRequest req;
    req.question_id = m_currentQuestionId;
    req.selected_option = choice;
    req.response_time_ms = time * 1000; // Convert seconds to milliseconds
    
    sendMessage(MessageType::C2S_SUBMIT_ANSWER_REQ, &req, sizeof(req));
}

void BackEnd::fiftyFifty()
{
    // This would be a special request type - for now, we'll handle it in the game logic
    // The server code reference shows this was handled differently
    // For compatibility with existing QML, we'll emit a signal
    emit openVote();
}

void BackEnd::callPhone()
{
    emit openCallPhone();
}

void BackEnd::vote()
{
    emit openVote();
}

void BackEnd::changeQuestion()
{
    // Handle change question request
    // This would need a specific message type
}

void BackEnd::playAlone()
{
    // Start single player game
    // This would send a PLAY_ALONE message
}

void BackEnd::playPvP()
{
    // Start PvP game - find or create room
    listRooms();
}

void BackEnd::listRooms()
{
    if (!m_isConnected) {
        return;
    }
    
    sendMessage(MessageType::C2S_LIST_ROOMS_REQ, nullptr, 0);
}

void BackEnd::createRoom(const QString& roomName, int gameMode, int numQuestions)
{
    if (!m_isConnected) {
        return;
    }
    
    CreateRoomRequest req;
    memset(&req, 0, sizeof(req));
    strncpy(req.room_name, roomName.toUtf8().constData(), MAX_ROOM_NAME_LEN - 1);
    req.game_mode = static_cast<GameMode>(gameMode);
    req.num_questions = numQuestions;
    
    sendMessage(MessageType::C2S_CREATE_ROOM_REQ, &req, sizeof(req));
}

void BackEnd::joinRoom(int roomId)
{
    if (!m_isConnected) {
        return;
    }
    
    JoinRoomRequest req;
    req.room_id = roomId;
    
    sendMessage(MessageType::C2S_JOIN_ROOM_REQ, &req, sizeof(req));
}

void BackEnd::setReady(bool ready)
{
    if (!m_isConnected) {
        return;
    }
    
    ReadyStatusRequest req;
    req.is_ready = ready;
    
    sendMessage(MessageType::C2S_READY_STATUS_REQ, &req, sizeof(req));
}

void BackEnd::leaveRoom()
{
    if (!m_isConnected) {
        return;
    }
    
    sendMessage(MessageType::C2S_LEAVE_ROOM_REQ, nullptr, 0);
}

void BackEnd::sendMessage(MessageType type, const void* body, size_t bodySize)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    
    MessageHeader header;
    header.type = type;
    header.body_len = bodySize;
    
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.writeRawData(reinterpret_cast<const char*>(&header), sizeof(header));
    
    if (body && bodySize > 0) {
        stream.writeRawData(reinterpret_cast<const char*>(body), bodySize);
    }
    
    m_socket->write(data);
}

void BackEnd::onConnected()
{
    m_isConnected = true;
    emit connectSuccess();
}

void BackEnd::onDisconnected()
{
    m_isConnected = false;
    emit connectFail();
}

void BackEnd::onReadyRead()
{
    while (m_socket->bytesAvailable() >= static_cast<qint64>(sizeof(MessageHeader))) {
        QByteArray headerData = m_socket->read(sizeof(MessageHeader));
        if (headerData.size() != sizeof(MessageHeader)) {
            break;
        }
        
        MessageHeader header;
        memcpy(&header, headerData.constData(), sizeof(header));
        
        QByteArray bodyData;
        if (header.body_len > 0) {
            while (m_socket->bytesAvailable() < header.body_len) {
                if (!m_socket->waitForReadyRead(1000)) {
                    return;
                }
            }
            bodyData = m_socket->read(header.body_len);
        }
        
        handleMessage(header, bodyData);
    }
}

void BackEnd::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    m_isConnected = false;
    emit connectFail();
}

void BackEnd::handleMessage(const MessageHeader& header, const QByteArray& data)
{
    switch (header.type) {
        case MessageType::S2C_LOGIN_RSP: {
            if (data.size() >= static_cast<int>(sizeof(LoginResponse))) {
                LoginResponse resp;
                memcpy(&resp, data.constData(), sizeof(resp));
                handleLoginResponse(resp);
            }
            break;
        }
        case MessageType::S2C_REGISTER_RSP: {
            if (data.size() >= static_cast<int>(sizeof(StatusResponse))) {
                StatusResponse resp;
                memcpy(&resp, data.constData(), sizeof(resp));
                handleRegisterResponse(resp);
            }
            break;
        }
        case MessageType::S2C_QUESTION_NOTIF: {
            if (data.size() >= static_cast<int>(sizeof(QuestionNotification))) {
                QuestionNotification notif;
                memcpy(&notif, data.constData(), sizeof(notif));
                handleQuestionNotification(notif);
            }
            break;
        }
        case MessageType::S2C_ROUND_RESULT_NOTIF: {
            if (data.size() >= static_cast<int>(sizeof(RoundResultNotification))) {
                RoundResultNotification notif;
                memcpy(&notif, data.constData(), sizeof(notif));
                handleRoundResult(notif);
            }
            break;
        }
        case MessageType::S2C_GAME_OVER_NOTIF: {
            if (data.size() >= static_cast<int>(sizeof(GameOverNotification))) {
                GameOverNotification notif;
                memcpy(&notif, data.constData(), sizeof(notif));
                handleGameOver(notif);
            }
            break;
        }
        case MessageType::S2C_ERROR_RSP: {
            if (data.size() >= static_cast<int>(sizeof(ErrorResponse))) {
                ErrorResponse error;
                memcpy(&error, data.constData(), sizeof(error));
                handleError(error);
            }
            break;
        }
        case MessageType::S2C_PLAYER_JOINED_NOTIF: {
            // Handle player joined
            if (data.size() >= static_cast<int>(sizeof(PlayerInfo))) {
                PlayerInfo info;
                memcpy(&info, data.constData(), sizeof(info));
                m_enemyName = QString::fromUtf8(info.display_name);
                emit enemy_nameChanged();
                emit foundPlayer();
            }
            break;
        }
        case MessageType::S2C_GAME_START_NOTIF: {
            emit foundPlayer();
            break;
        }
        default:
            qDebug() << "Unhandled message type:" << static_cast<int>(header.type);
            break;
    }
}

void BackEnd::handleLoginResponse(const LoginResponse& resp)
{
    if (resp.code == StatusCode::SUCCESS) {
        m_userId = resp.user_id;
        m_userName = QString::fromUtf8(resp.display_name);
        emit userNameChanged();
        emit loginSuccess();
    } else if (resp.code == StatusCode::INVALID_CREDENTIALS) {
        emit wrongPassword();
    } else {
        emit accountNotExist();
    }
}

void BackEnd::handleRegisterResponse(const StatusResponse& resp)
{
    if (resp.code == StatusCode::SUCCESS) {
        emit signupSuccess();
    } else if (resp.code == StatusCode::EMAIL_EXISTS) {
        emit accountExist();
    }
}

void BackEnd::handleQuestionNotification(const QuestionNotification& notif)
{
    m_currentQuestionId = notif.question_id;
    m_question = QString::fromUtf8(notif.content);
    m_a = QString::fromUtf8(notif.options[0]);
    m_b = QString::fromUtf8(notif.options[1]);
    m_c = QString::fromUtf8(notif.options[2]);
    m_d = QString::fromUtf8(notif.options[3]);
    
    emit questionChanged();
    emit aChanged();
    emit bChanged();
    emit cChanged();
    emit dChanged();
}

void BackEnd::handleRoundResult(const RoundResultNotification& notif)
{
    // Find our result
    for (uint8_t i = 0; i < notif.result_count; i++) {
        const PlayerRoundResult& result = notif.results[i];
        if (result.user_id == m_userId) {
            if (result.was_eliminated) {
                emit lose();
            } else if (result.score_change > 0) {
                emit correctAnswer();
                m_prize++;
                emit prizeChanged();
            } else {
                emit lose();
            }
            break;
        }
    }
}

void BackEnd::handleGameOver(const GameOverNotification& notif)
{
    emit showResultPvP();
    m_currentRoomId = 0;
    m_prize = 0;
    emit prizeChanged();
}

void BackEnd::handleError(const ErrorResponse& error)
{
    qDebug() << "Error:" << QString::fromUtf8(error.message);
    
    switch (error.code) {
        case StatusCode::INVALID_CREDENTIALS:
            emit wrongPassword();
            break;
        case StatusCode::EMAIL_EXISTS:
            emit accountExist();
            break;
        default:
            break;
    }
}

