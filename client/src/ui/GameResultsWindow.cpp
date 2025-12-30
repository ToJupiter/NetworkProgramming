#include "GameResultsWindow.h"
#include "ui_GameResultsWindow.h"
#include "../models/SessionState.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFont>

GameResultsWindow::GameResultsWindow(const QVector<PlayerResult>& results,
                                    GameEndReason endReason,
                                    uint8_t winnerCount,
                                    QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameResultsWindow)
    , playerResults(results)
    , gameEndReason(endReason)
    , numWinners(winnerCount)
{
    ui->setupUi(this);
    setupUI();
    displayResults();
}

GameResultsWindow::~GameResultsWindow() {
    delete ui;
}

void GameResultsWindow::setupUI() {
    setWindowTitle("Game Results");
    
    // Configure results table
    ui->tblResults->setColumnCount(6);
    ui->tblResults->setHorizontalHeaderLabels({
        "Rank", "Player", "Score", "Correct Answers", "Avg Response Time (ms)", "Status"
    });
    ui->tblResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblResults->setSelectionMode(QAbstractItemView::NoSelection);
    
    // Connect buttons
    connect(ui->btnLeaveRoom, &QPushButton::clicked, this, &GameResultsWindow::onLeaveRoomClicked);
    connect(ui->btnStayInRoom, &QPushButton::clicked, this, &GameResultsWindow::onStayInRoomClicked);
    
    // Set title
    ui->lblTitle->setText(getEndReasonText());
    
    // Style the title
    QFont titleFont = ui->lblTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    ui->lblTitle->setFont(titleFont);
    ui->lblTitle->setAlignment(Qt::AlignCenter);
}

void GameResultsWindow::displayResults() {
    SessionState* sessionState = &SessionState::instance();
    uint32_t localUserId = sessionState->getUserId();
    
    ui->tblResults->setRowCount(0);
    
    for (const auto& result : playerResults) {
        int row = ui->tblResults->rowCount();
        ui->tblResults->insertRow(row);
        
        auto* rankItem = new QTableWidgetItem(QString::number(result.final_rank));
        auto* nameItem = new QTableWidgetItem(result.display_name);
        auto* scoreItem = new QTableWidgetItem(QString::number(result.final_score));
        auto* correctItem = new QTableWidgetItem(QString::number(result.correct_answers));
        auto* avgTimeItem = new QTableWidgetItem(QString::number(result.avg_response_time_ms));
        
        QString statusText;
        if (result.is_winner) {
            statusText = "🏆 Winner";
        } else {
            statusText = "Participant";
        }
        auto* statusItem = new QTableWidgetItem(statusText);
        
        // Highlight local player
        if (result.user_id == localUserId) {
            QFont boldFont;
            boldFont.setBold(true);
            rankItem->setFont(boldFont);
            nameItem->setFont(boldFont);
            scoreItem->setFont(boldFont);
            correctItem->setFont(boldFont);
            avgTimeItem->setFont(boldFont);
            statusItem->setFont(boldFont);
            
            QColor highlightColor(255, 250, 205); // Light yellow
            rankItem->setBackground(QBrush(highlightColor));
            nameItem->setBackground(QBrush(highlightColor));
            scoreItem->setBackground(QBrush(highlightColor));
            correctItem->setBackground(QBrush(highlightColor));
            avgTimeItem->setBackground(QBrush(highlightColor));
            statusItem->setBackground(QBrush(highlightColor));
        }
        
        // Highlight winners with gold background
        if (result.is_winner) {
            QColor winnerColor(255, 215, 0, 50); // Semi-transparent gold
            rankItem->setBackground(QBrush(winnerColor));
            nameItem->setBackground(QBrush(winnerColor));
            scoreItem->setBackground(QBrush(winnerColor));
            correctItem->setBackground(QBrush(winnerColor));
            avgTimeItem->setBackground(QBrush(winnerColor));
            statusItem->setBackground(QBrush(winnerColor));
        }
        
        ui->tblResults->setItem(row, 0, rankItem);
        ui->tblResults->setItem(row, 1, nameItem);
        ui->tblResults->setItem(row, 2, scoreItem);
        ui->tblResults->setItem(row, 3, correctItem);
        ui->tblResults->setItem(row, 4, avgTimeItem);
        ui->tblResults->setItem(row, 5, statusItem);
    }
}

QString GameResultsWindow::getEndReasonText() const {
    switch (gameEndReason) {
        case GameEndReason::SINGLE_WINNER:
            return "🎉 Game Over - We Have a Winner! 🎉";
        case GameEndReason::DRAW:
            return QString("🎉 Game Over - It's a Draw! (%1 Winners) 🎉").arg(numWinners);
        case GameEndReason::NO_WINNER_WIPEOUT:
            return "💥 Game Over - Total Wipeout! No Winners! 💥";
        default:
            return "Game Over";
    }
}

void GameResultsWindow::onLeaveRoomClicked() {
    emit leaveRoomRequested();
    close();
}

void GameResultsWindow::onStayInRoomClicked() {
    emit stayInRoomRequested();
    close();
}
