#include "RoomWindow.h"
#include "ui_RoomWindow.h"
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
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name))
        .arg(currentRoom.current_players)
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
    
    // For game start notifications
    connect(networkManager, &NetworkManager::gameStartNotif,
            this, &RoomWindow::onGameStarted);
    
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

    // TODO: Add sendStartGame() to NetworkManager or use alternative message
    // For now, just show message
    QMessageBox::information(this, "Starting Game", "Game will start when all players are ready!");
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
    // TODO: Phase 5 - Transition to GameWindow
    QMessageBox::information(this, "Game Started", "The game has started!");
    stopAutoRefresh();
    stopCountdownTimer();
}

void RoomWindow::onPlayerListUpdate(uint8_t playerCount, const QVector<PlayerInfo>& players)
{
    cachedPlayers = players;
    populatePlayerTable(players);

    // Update header
    QString modeStr = (currentRoom.game_mode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    QString headerText = QString("Room: %1 - Players: %2/%3 - Mode: %4")
        .arg(QString::fromLatin1(currentRoom.room_name))
        .arg(playerCount)
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
    // Add player to list
    int rowCount = ui->tblPlayers->rowCount();
    ui->tblPlayers->insertRow(rowCount);

    QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromLatin1(player.display_name));
    QTableWidgetItem *statusItem = new QTableWidgetItem(formatPlayerStatus(player.is_ready, true));
    QTableWidgetItem *roleItem = new QTableWidgetItem(formatPlayerRole(player.user_id));

    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
    roleItem->setFlags(roleItem->flags() & ~Qt::ItemIsEditable);

    ui->tblPlayers->setItem(rowCount, 0, nameItem);
    ui->tblPlayers->setItem(rowCount, 1, statusItem);
    ui->tblPlayers->setItem(rowCount, 2, roleItem);

    cachedPlayers.append(player);
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
}

void RoomWindow::onPlayerReadyChanged(uint32_t userId, bool isReady)
{
    // Update player ready status in table
    for (int row = 0; row < ui->tblPlayers->rowCount(); ++row) {
        // Update status column
        QTableWidgetItem *statusItem = ui->tblPlayers->item(row, 1);
        if (statusItem) {
            statusItem->setText(isReady ? "Ready ✓" : "Not Ready");
        }
    }

    // Update cached players
    for (auto& player : cachedPlayers) {
        if (player.user_id == userId) {
            player.is_ready = isReady;
            break;
        }
    }

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

    for (const auto& player : players) {
        int rowCount = ui->tblPlayers->rowCount();
        ui->tblPlayers->insertRow(rowCount);

        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromLatin1(player.display_name));
        QTableWidgetItem *statusItem = new QTableWidgetItem(formatPlayerStatus(player.is_ready, true));
        QTableWidgetItem *roleItem = new QTableWidgetItem(formatPlayerRole(player.user_id));

        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        roleItem->setFlags(roleItem->flags() & ~Qt::ItemIsEditable);

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
