#include "GameHistoryWindow.h"
#include "ui_GameHistoryWindow.h"
#include "../network/NetworkManager.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>

GameHistoryWindow::GameHistoryWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameHistoryWindow)
    , networkManager(&NetworkManager::instance())
{
    ui->setupUi(this);
    setWindowTitle("Game History");
    
    connect(networkManager, &NetworkManager::gameHistoryResponse,
            this, &GameHistoryWindow::onGameHistoryReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &GameHistoryWindow::onConnectionError);
    
    ui->tblHistory->setColumnCount(7);
    ui->tblHistory->setHorizontalHeaderLabels({
        "Date", "Mode", "Score", "Rank", "Correct", "Total", "Avg Time (ms)"
    });
    ui->tblHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    loadHistory();
}

GameHistoryWindow::~GameHistoryWindow() {
    delete ui;
}

void GameHistoryWindow::loadHistory() {
    ui->lblStatus->setText("Loading game history...");
    ui->tblHistory->setRowCount(0);
    networkManager->sendGetGameHistory();
}

void GameHistoryWindow::onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries) {
    if (status == StatusCode::SUCCESS) {
        displayHistory(entries);
    } else {
        ui->lblStatus->setText("Failed to load game history");
    }
}

void GameHistoryWindow::displayHistory(const QVector<GameHistoryEntry>& entries) {
    ui->tblHistory->setRowCount(0);
    
    for (const auto& entry : entries) {
        int row = ui->tblHistory->rowCount();
        ui->tblHistory->insertRow(row);
        
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(entry.timestamp_sec);
        QString dateStr = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        
        auto* dateItem = new QTableWidgetItem(dateStr);
        auto* modeItem = new QTableWidgetItem(QString::fromLatin1(entry.game_mode));
        auto* scoreItem = new QTableWidgetItem(QString::number(entry.player_score));
        auto* rankItem = new QTableWidgetItem(entry.player_rank > 0 ? QString::number(entry.player_rank) : "—");
        auto* correctItem = new QTableWidgetItem(QString("%1/%2").arg(entry.correct_answers).arg(entry.total_questions));
        auto* totalItem = new QTableWidgetItem(QString::number(entry.total_questions));
        auto* timeItem = new QTableWidgetItem(QString::number(entry.avg_response_time_ms));
        
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        modeItem->setFlags(modeItem->flags() & ~Qt::ItemIsEditable);
        scoreItem->setFlags(scoreItem->flags() & ~Qt::ItemIsEditable);
        rankItem->setFlags(rankItem->flags() & ~Qt::ItemIsEditable);
        correctItem->setFlags(correctItem->flags() & ~Qt::ItemIsEditable);
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        
        ui->tblHistory->setItem(row, 0, dateItem);
        ui->tblHistory->setItem(row, 1, modeItem);
        ui->tblHistory->setItem(row, 2, scoreItem);
        ui->tblHistory->setItem(row, 3, rankItem);
        ui->tblHistory->setItem(row, 4, correctItem);
        ui->tblHistory->setItem(row, 5, totalItem);
        ui->tblHistory->setItem(row, 6, timeItem);
    }
    
    ui->lblStatus->setText(QString("Showing %1 games").arg(entries.size()));
}

void GameHistoryWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}
