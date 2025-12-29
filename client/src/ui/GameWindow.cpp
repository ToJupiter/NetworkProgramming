#include "ui_GameWindow.h"
#include "GameWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"

#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPalette>
#include <algorithm>

namespace {
QString fromFixed(const char* buf, size_t len) {
    return QString::fromLatin1(buf, static_cast<int>(len)).split('\0').first();
}
}

GameWindow::GameWindow(GameMode mode, uint32_t roomId, uint32_t hostUserId,
                       const QVector<PlayerInfo>& initialPlayers, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , networkManager(&NetworkManager::instance())
    , sessionState(&SessionState::instance())
    , gameMode(mode)
    , roomId(roomId)
    , hostUserId(hostUserId)
    , questionTimer(new QTimer(this))
    , timeRemainingSec(0)
    , currentQuestionId(0)
    , questionNumber(0)
    , questionActive(false)
    , answerSent(false)
    , paused(false)
{
    ui->setupUi(this);
    setupUiTheme();
    bindSignals();

    // Seed players list
    players.clear();
    players.reserve(initialPlayers.size());
    for (const auto& p : initialPlayers) {
        ClientPlayer cp;
        cp.user_id = p.user_id;
        cp.display_name = fromFixed(p.display_name, sizeof(p.display_name));
        cp.is_ready = p.is_ready;
        cp.score = 0;
        cp.is_eliminated = false;
        players.append(cp);
    }

    // Header info
    ui->lblRoomTitle->setText(QString("Room: %1").arg(roomId));
    ui->lblMode->setText(QString("Mode: %1").arg(gameMode == GameMode::ELIMINATION ? "Elimination" : "Scoring"));
    ui->lblQuestionCounter->setText("Question 0/0");
    ui->lblTimer->setText("Timer: --");
    ui->progressTimer->setMaximum(100);
    ui->progressTimer->setValue(0);
    ui->lblFeedback->setText("Waiting for game start...");

    // Scoreboard setup
    ui->tblScoreboard->setColumnCount(4);
    ui->tblScoreboard->setHorizontalHeaderLabels({"Rank", "Player", "Score", "Status"});
    ui->tblScoreboard->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblScoreboard->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblScoreboard->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblScoreboard->setSelectionMode(QAbstractItemView::NoSelection);
    updateScoreboard();

    // Disable answer buttons until a question arrives
    setButtonsEnabled(false);

    connect(questionTimer, &QTimer::timeout, this, &GameWindow::onQuestionTick);
}

GameWindow::~GameWindow() {
    stopQuestionTimer();
    delete ui;
}

void GameWindow::setupUiTheme() {
    // Simple Kahoot-like color blocks for the four buttons
    ui->btnOptionA->setStyleSheet("background:#e74c3c; color:white; font-size:16px; font-weight:bold;");
    ui->btnOptionB->setStyleSheet("background:#2980b9; color:white; font-size:16px; font-weight:bold;");
    ui->btnOptionC->setStyleSheet("background:#f1c40f; color:black; font-size:16px; font-weight:bold;");
    ui->btnOptionD->setStyleSheet("background:#27ae60; color:white; font-size:16px; font-weight:bold;");
}

void GameWindow::bindSignals() {
    connect(ui->btnOptionA, &QPushButton::clicked, this, &GameWindow::onOptionAClicked);
    connect(ui->btnOptionB, &QPushButton::clicked, this, &GameWindow::onOptionBClicked);
    connect(ui->btnOptionC, &QPushButton::clicked, this, &GameWindow::onOptionCClicked);
    connect(ui->btnOptionD, &QPushButton::clicked, this, &GameWindow::onOptionDClicked);
    connect(ui->btnPauseGame, &QPushButton::clicked, this, &GameWindow::onPauseGameClicked);
    connect(ui->btnResumeGame, &QPushButton::clicked, this, &GameWindow::onResumeGameClicked);

    // Network signals
    connect(networkManager, &NetworkManager::questionNotif,
            this, &GameWindow::onQuestionReceived);
    connect(networkManager, &NetworkManager::roundResultNotif,
            this, &GameWindow::onRoundResult);
    connect(networkManager, &NetworkManager::playerEliminatedNotif,
            this, &GameWindow::onPlayerEliminated);
    connect(networkManager, &NetworkManager::gameOverNotif,
            this, &GameWindow::onGameOver);
    connect(networkManager, &NetworkManager::gamePausedNotif,
            this, &GameWindow::onGamePaused);
    connect(networkManager, &NetworkManager::gameResumedNotif,
            this, &GameWindow::onGameResumed);
    connect(networkManager, &NetworkManager::gameTerminatedNotif,
            this, &GameWindow::onGameTerminated);
    connect(networkManager, &NetworkManager::connectionError,
            this, &GameWindow::onConnectionError);
    connect(networkManager, &NetworkManager::returnToRoomResponse,
            this, [this](StatusCode code) {
                if (code == StatusCode::SUCCESS) {
                    emit returnedToRoom();
                }
            });
}

void GameWindow::setButtonsEnabled(bool enabled) {
    ui->btnOptionA->setEnabled(enabled && !paused);
    ui->btnOptionB->setEnabled(enabled && !paused);
    ui->btnOptionC->setEnabled(enabled && !paused);
    ui->btnOptionD->setEnabled(enabled && !paused);
}

void GameWindow::onPauseGameClicked() {
    if (!paused && hostUserId == sessionState->getUserId()) {
        networkManager->sendPauseGame();
    }
}

void GameWindow::onResumeGameClicked() {
    if (paused && hostUserId == sessionState->getUserId()) {
        networkManager->sendResumeGame();
    }
}

void GameWindow::startQuestion(uint32_t questionId, const QString& content,
                               const QStringList& options, uint32_t timeLimitSec) {
    questionActive = true;
    answerSent = false;
    currentQuestionId = questionId;
    questionNumber += 1;

    ui->lblQuestion->setText(content);
    ui->lblQuestionCounter->setText(QString("Question %1").arg(questionNumber));
    ui->lblFeedback->setText("Pick an answer before time runs out.");

    ui->btnOptionA->setText(QString("A) %1").arg(options.value(0)));
    ui->btnOptionB->setText(QString("B) %1").arg(options.value(1)));
    ui->btnOptionC->setText(QString("C) %1").arg(options.value(2)));
    ui->btnOptionD->setText(QString("D) %1").arg(options.value(3)));

    resetOptionStyles();
    setButtonsEnabled(true);

    startQuestionTimer(timeLimitSec);
    answerElapsed.restart();
}

void GameWindow::startQuestionTimer(uint32_t seconds) {
    timeRemainingSec = static_cast<int>(seconds);
    if (timeRemainingSec < 0) timeRemainingSec = 0;
    ui->lblTimer->setText(QString("Timer: %1").arg(timeRemainingSec));
    ui->progressTimer->setMaximum(std::max(1, timeRemainingSec));
    ui->progressTimer->setValue(timeRemainingSec);

    questionTimer->setInterval(1000);
    questionTimer->start();
}

void GameWindow::stopQuestionTimer() {
    if (questionTimer) {
        questionTimer->stop();
    }
}

void GameWindow::updateTimerUi() {
    ui->lblTimer->setText(QString("Timer: %1").arg(timeRemainingSec));
    ui->progressTimer->setValue(std::max(0, timeRemainingSec));
}

void GameWindow::onQuestionReceived(uint32_t questionId, const QString& content,
                                    const QStringList& options, uint32_t timeLimitSec) {
    // Reset paused banner
    ui->lblGameStatus->clear();
    paused = false;
    setButtonsEnabled(true);

    startQuestion(questionId, content, options, timeLimitSec);
}

void GameWindow::handleAnswerSelection(int optionIndex) {
    if (!questionActive || answerSent || paused) return;
    answerSent = true;
    setButtonsEnabled(false);

    // optionIndex is 1-based for protocol compatibility
    qDebug().nospace() << "Player " << sessionState->getUserId()
                       << " selected answer " << optionIndex
                       << " for Question " << currentQuestionId;
    uint32_t elapsedMs = static_cast<uint32_t>(answerElapsed.elapsed());
    networkManager->sendSubmitAnswer(currentQuestionId, static_cast<uint8_t>(optionIndex), elapsedMs);

    markSelection(optionIndex);
    setFeedback("Answer submitted. Waiting for results...");
}

void GameWindow::onOptionAClicked() { handleAnswerSelection(1); }
void GameWindow::onOptionBClicked() { handleAnswerSelection(2); }
void GameWindow::onOptionCClicked() { handleAnswerSelection(3); }
void GameWindow::onOptionDClicked() { handleAnswerSelection(4); }

void GameWindow::onQuestionTick() {
    if (timeRemainingSec > 0) {
        timeRemainingSec -= 1;
        updateTimerUi();
    }
    if (timeRemainingSec <= 0) {
        stopQuestionTimer();
        questionActive = false;
        setButtonsEnabled(false);
        setFeedback("Time's up. Waiting for results...");
    }
}

void GameWindow::markSelection(int optionIndex) {
    QPushButton* buttons[4] = {ui->btnOptionA, ui->btnOptionB, ui->btnOptionC, ui->btnOptionD};
    for (int i = 0; i < 4; ++i) {
        if (i == optionIndex - 1) {
            buttons[i]->setStyleSheet(buttons[i]->styleSheet() + "border: 3px solid #2c3e50;");
        }
    }
}

void GameWindow::highlightCorrect(uint8_t correctOption) {
    QPushButton* buttons[4] = {ui->btnOptionA, ui->btnOptionB, ui->btnOptionC, ui->btnOptionD};
    for (int i = 0; i < 4; ++i) {
        QString base = buttons[i]->styleSheet();
        if (i == static_cast<int>(correctOption) - 1) {
            buttons[i]->setStyleSheet(base + "border: 3px solid #27ae60;");
        } else if (answerSent && i != static_cast<int>(correctOption) - 1) {
            buttons[i]->setStyleSheet(base + "opacity: 0.6;");
        }
    }
}

void GameWindow::resetOptionStyles() {
    setupUiTheme();
}

void GameWindow::applyRoundResults(const QVector<PlayerRoundResult>& results) {
    for (const auto& res : results) {
        for (auto& p : players) {
            if (p.user_id == res.user_id) {
                p.score = res.total_score;
                p.is_eliminated = res.was_eliminated;
                break;
            }
        }
    }
    updateScoreboard();
}

void GameWindow::onRoundResult(uint8_t correctOption, uint8_t /*playerCount*/,
                               const QVector<PlayerRoundResult>& results) {
    questionActive = false;
    stopQuestionTimer();
    highlightCorrect(correctOption == 0 ? 1 : correctOption);
    applyRoundResults(results);

    QString msg = QString("Correct answer: %1").arg(correctOption);
    setFeedback(msg);
}

void GameWindow::onPlayerEliminated(uint32_t userId) {
    for (auto& p : players) {
        if (p.user_id == userId) {
            p.is_eliminated = true;
            break;
        }
    }
    updateScoreboard();
}

void GameWindow::onGameOver(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings, 
                            uint8_t gameEndReason, uint8_t winnerCount) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    questionActive = false;

    QString text;
    
    // 0 = SINGLE_WINNER, 1 = DRAW, 2 = NO_WINNER_WIPEOUT
    if (gameEndReason == 2) {
        // No Winner - Everyone eliminated
        text = "Game Over! No Winner - Everyone answered the last question incorrectly!";
    } else if (gameEndReason == 1) {
        // Draw - Multiple winners
        QStringList winners;
        for (const auto& p : rankings) {
            if (p.is_winner) {
                winners.append(QString::fromLatin1(p.display_name, MAX_DISPLAY_NAME_LEN));
            }
        }
        text = QString("Game Over! It's a Draw!\nWinners: %1").arg(winners.join(", "));
    } else {
        // Single Winner
        QString winner = rankingCount > 0 ? QString::fromLatin1(rankings[0].display_name, MAX_DISPLAY_NAME_LEN) : "";
        text = QString("Game over!%1").arg(winner.isEmpty() ? QString() : QString(" Winner: %1").arg(winner));
    }
    
    QMessageBox::information(this, "Game Over", text);

    // Send return to room request instead of closing
    networkManager->sendReturnToRoom();
}

void GameWindow::onGamePaused() {
    paused = true;
    stopQuestionTimer();
    setButtonsEnabled(false);
    ui->lblGameStatus->setText("Game paused");
    ui->btnPauseGame->setVisible(false);
    ui->btnResumeGame->setVisible(true);
}

void GameWindow::onGameResumed() {
    paused = false;
    ui->lblGameStatus->clear();
    ui->btnPauseGame->setVisible(true);
    ui->btnResumeGame->setVisible(false);
    if (questionActive && timeRemainingSec > 0) {
        setButtonsEnabled(true);
        questionTimer->start();
    }
}

void GameWindow::onGameTerminated(TerminationReason reason) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    QString why;
    switch (reason) {
        case TerminationReason::HOST_LEFT: why = "Host left the match"; break;
        case TerminationReason::NOT_ENOUGH_PLAYERS: why = "Not enough players"; break;
        case TerminationReason::SERVER_SHUTDOWN: why = "Server shutdown"; break;
        default: why = "Match terminated"; break;
    }
    QMessageBox::warning(this, "Game Terminated", why);
    sessionState->setCurrentRoomId(0);
    close();
}

void GameWindow::onConnectionError(const QString& error) {
    stopQuestionTimer();
    setButtonsEnabled(false);
    QMessageBox::critical(this, "Connection Error", error);
}

void GameWindow::onReturnedToRoom() {
    // No-op: RoomWindow handles closing this window and showing room view
}

void GameWindow::updateScoreboard() {
    // Sort players by score descending, non-eliminated first
    QVector<ClientPlayer> sorted = players;
    std::sort(sorted.begin(), sorted.end(), [](const ClientPlayer& a, const ClientPlayer& b) {
        if (a.is_eliminated != b.is_eliminated) return !a.is_eliminated && b.is_eliminated;
        return a.score > b.score;
    });

    ui->tblScoreboard->setRowCount(0);
    for (int i = 0; i < sorted.size(); ++i) {
        const auto& p = sorted[i];
        int row = ui->tblScoreboard->rowCount();
        ui->tblScoreboard->insertRow(row);

        auto *rankItem = new QTableWidgetItem(QString::number(i + 1));
        auto *nameItem = new QTableWidgetItem(p.display_name);
        auto *scoreItem = new QTableWidgetItem(QString::number(p.score));
        auto *statusItem = new QTableWidgetItem(formatStatus(p));

        rankItem->setFlags(rankItem->flags() & ~Qt::ItemIsEditable);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        scoreItem->setFlags(scoreItem->flags() & ~Qt::ItemIsEditable);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);

        // Highlight current user for clarity (bold + subtle background)
        if (p.user_id == sessionState->getUserId()) {
            QFont bold;
            bold.setBold(true);
            rankItem->setFont(bold);
            nameItem->setFont(bold);
            scoreItem->setFont(bold);
            statusItem->setFont(bold);

            QColor bg(230, 240, 255); // soft blue tint
            rankItem->setBackground(QBrush(bg));
            nameItem->setBackground(QBrush(bg));
            scoreItem->setBackground(QBrush(bg));
            statusItem->setBackground(QBrush(bg));
        }

        ui->tblScoreboard->setItem(row, 0, rankItem);
        ui->tblScoreboard->setItem(row, 1, nameItem);
        ui->tblScoreboard->setItem(row, 2, scoreItem);
        ui->tblScoreboard->setItem(row, 3, statusItem);
    }
}

QString GameWindow::formatStatus(const ClientPlayer& p) const {
    QStringList bits;
    if (p.user_id == hostUserId) bits << "Host";
    if (p.user_id == sessionState->getUserId()) bits << "You";
    if (p.is_eliminated) bits << "Eliminated";
    return bits.isEmpty() ? "Player" : bits.join(", ");
}

void GameWindow::setFeedback(const QString& text, bool warn) {
    ui->lblFeedback->setText(text);
    ui->lblFeedback->setStyleSheet(warn ? "color:#c0392b;" : "");
}
