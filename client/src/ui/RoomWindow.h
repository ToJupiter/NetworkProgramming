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
};

#endif // ROOMWINDOW_H
