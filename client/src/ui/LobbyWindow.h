#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "protocol.h"

class NetworkManager;
class RoomWindow;

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
    QTimer *refreshTimer;
    QVector<RoomInfo> cachedRooms;
    int selectedRoomIndex;
    bool joinInProgress;
};

#endif // LOBBYWINDOW_H
