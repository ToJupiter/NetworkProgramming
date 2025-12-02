#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QThread>
#include "../common/protocol.h"

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged)
    Q_PROPERTY(QString enemy_name READ enemy_name NOTIFY enemy_nameChanged)
    Q_PROPERTY(int prize READ prize NOTIFY prizeChanged)
    Q_PROPERTY(QString question READ question NOTIFY questionChanged)
    Q_PROPERTY(QString a READ a NOTIFY aChanged)
    Q_PROPERTY(QString b READ b NOTIFY bChanged)
    Q_PROPERTY(QString c READ c NOTIFY cChanged)
    Q_PROPERTY(QString d READ d NOTIFY dChanged)
    Q_PROPERTY(int enemy_current_question READ enemy_current_question NOTIFY enemy_current_questionChanged)
    Q_PROPERTY(int enemy_seconds READ enemy_seconds NOTIFY enemy_secondsChanged)
    Q_PROPERTY(QString enemy_status READ enemy_status NOTIFY enemy_statusChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    QString userName() const { return m_userName; }
    QString enemy_name() const { return m_enemyName; }
    int prize() const { return m_prize; }
    QString question() const { return m_question; }
    QString a() const { return m_a; }
    QString b() const { return m_b; }
    QString c() const { return m_c; }
    QString d() const { return m_d; }
    int enemy_current_question() const { return m_enemyCurrentQuestion; }
    int enemy_seconds() const { return m_enemySeconds; }
    QString enemy_status() const { return m_enemyStatus; }

public slots:
    void connectToServer();
    void disconnectToServer();
    void signIn(const QString& username, const QString& password);
    void signUp(const QString& username, const QString& password);
    void changePassword(const QString& newPassword);
    void choiceAnswer(int choice, int time);
    void fiftyFifty();
    void callPhone();
    void vote();
    void changeQuestion();
    void playAlone();
    void playPvP();
    void listRooms();
    void createRoom(const QString& roomName, int gameMode, int numQuestions);
    void joinRoom(int roomId);
    void setReady(bool ready);
    void leaveRoom();

signals:
    void connectSuccess();
    void connectFail();
    void loginSuccess();
    void loggedIn();
    void accountBlocked();
    void accountNotExist();
    void wrongPassword();
    void signupSuccess();
    void accountExist();
    void changePasswordSuccess();
    void sameOldPassword();
    void correctAnswer();
    void lose();
    void lose2();
    void foundPlayer();
    void openVote();
    void openCallPhone();
    void notFoundPlayer();
    void showResultPvP();
    
    void userNameChanged();
    void enemy_nameChanged();
    void prizeChanged();
    void questionChanged();
    void aChanged();
    void bChanged();
    void cChanged();
    void dChanged();
    void enemy_current_questionChanged();
    void enemy_secondsChanged();
    void enemy_statusChanged();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);

private:
    void sendMessage(MessageType type, const void* body, size_t bodySize);
    void handleMessage(const MessageHeader& header, const QByteArray& data);
    void handleLoginResponse(const LoginResponse& resp);
    void handleRegisterResponse(const StatusResponse& resp);
    void handleQuestionNotification(const QuestionNotification& notif);
    void handleRoundResult(const RoundResultNotification& notif);
    void handleGameOver(const GameOverNotification& notif);
    void handleError(const ErrorResponse& error);

    QTcpSocket* m_socket;
    QString m_userName;
    QString m_enemyName;
    int m_prize;
    QString m_question;
    QString m_a;
    QString m_b;
    QString m_c;
    QString m_d;
    int m_enemyCurrentQuestion;
    int m_enemySeconds;
    QString m_enemyStatus;
    uint32_t m_userId;
    uint32_t m_currentRoomId;
    uint32_t m_currentQuestionId;
    bool m_isConnected;
    
    QString m_serverHost;
    quint16 m_serverPort;
};

#endif // BACKEND_H


