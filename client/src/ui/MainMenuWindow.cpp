#include "MainMenuWindow.h"
#include "ui_MainMenuWindow.h"
#include "LobbyWindow.h"
#include "ReplayWindow.h"
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
    
    connect(networkManager, &NetworkManager::statsResponse,
            this, &MainMenuWindow::onStatsReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &MainMenuWindow::onConnectionError);
    
    ui->btnPlay->setStyleSheet("background:#27ae60; color:white; font-weight:bold; font-size:14px;");
    ui->btnViewStats->setStyleSheet("background:#3498db; color:white; font-weight:bold; font-size:14px;");
    ui->btnReplay->setStyleSheet("background:#9b59b6; color:white; font-weight:bold; font-size:14px;");
    
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
    QString statsText = QString(
        "<b>%1</b><br><br>"
        "<b>Ranked Points:</b> %2<br><br>"
        "<b>Elimination Mode:</b><br>"
        "Matches: %3 | Wins: %4 | High Score: %5<br>"
        "Accuracy: %6%<br><br>"
        "<b>Scoring Mode:</b><br>"
        "Matches: %7 | Wins: %8 | High Score: %9<br>"
        "Accuracy: %10%"
    ).arg(SessionState::instance().getDisplayName())
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

void MainMenuWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}
