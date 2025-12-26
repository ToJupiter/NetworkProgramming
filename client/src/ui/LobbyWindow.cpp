#include "LobbyWindow.h"
#include "ui_LobbyWindow.h"
#include "CreateRoomDialog.h"
#include "RoomWindow.h"
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
    , refreshTimer(new QTimer(this))
    , selectedRoomIndex(-1)
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

    // Network signals
    connect(networkManager, &NetworkManager::listRoomsResponse,
            this, &LobbyWindow::onListRoomsResponse);
    connect(networkManager, &NetworkManager::createRoomResponse,
            this, &LobbyWindow::onCreateRoomResponse);
    connect(networkManager, &NetworkManager::joinRoomResponse,
            this, &LobbyWindow::onJoinRoomResponse);
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
    QMessageBox::information(this, "Statistics",
        "Player stats view coming in Phase 5.\n\n"
        "Current features:\n"
        "- Join available rooms\n"
        "- Create new rooms\n"
        "- Play quiz games");
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
            selectedRoomIndex = row;
            RoomInfo room = cachedRooms[row];
            ui->lblStatus->setText(QString("Joining room '%1'...").arg(room.room_name));

            // Send join request
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
        onRefreshClicked(); // Refresh room list
    } else {
        QMessageBox::critical(this, "Create Room Failed",
            QString("Failed to create room. Error code: %1").arg((uint8_t)code));
    }
}

void LobbyWindow::onJoinRoomResponse(StatusCode code, const RoomInfo& room_info,
                                     uint8_t player_count, const QVector<PlayerInfo>& players,
                                     uint32_t host_user_id) {
    if (code == StatusCode::SUCCESS) { // Success
        // Store room info in session
        SessionState::instance().setCurrentRoomId(room_info.room_id);

        QMessageBox::information(this, "Room Joined",
            QString("Joined room '%1'!\n"
                    "Players: %2")
                .arg(room_info.room_name)
                .arg(player_count));

        // Transition to RoomWindow (Phase 4)
        if (!roomWindow) {
            roomWindow = new RoomWindow(room_info, host_user_id, players, this);
        }
        this->hide();
        roomWindow->show();
    } else {
        QMessageBox::critical(this, "Join Room Failed",
            QString("Failed to join room. Error code: %1").arg((uint8_t)code));
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

        // Room Name
        auto *itemName = new QTableWidgetItem(room.room_name);
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

    // Connect table click signal
    connect(ui->tblRooms, &QTableWidget::cellClicked, this, &LobbyWindow::onRoomTableItemClicked);
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
