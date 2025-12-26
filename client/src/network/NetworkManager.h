#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include "protocol.h"

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
    void sendGetStats();
    void sendPauseGame();
    void sendResumeGame();
    
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
    void gameOverNotif(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings);
    void gamePausedNotif();
    void gameResumedNotif();
    void gameTerminatedNotif(TerminationReason reason);
    
    void statsResponse(const UserStatsResponse& stats);
    void errorResponse(StatusCode code, const QString& message);
    
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    
private:
    NetworkManager();
    ~NetworkManager();
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    void sendMessage(MessageType type, const QByteArray& body);
    void processBuffer();
    void handleMessage(MessageType type, const QByteArray& body);
    
    QTcpSocket* m_socket;
    QByteArray m_recvBuffer;
};
