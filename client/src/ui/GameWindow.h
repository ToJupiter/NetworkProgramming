#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QStringList>
#include "protocol.h"
#include "../models/GameState.h"

namespace Ui {
class GameWindow;
}

class NetworkManager;
class SessionState;

class GameWindow : public QMainWindow {
    Q_OBJECT
public:
    GameWindow(GameMode mode, uint32_t roomId, uint32_t hostUserId,
               const QVector<PlayerInfo>& players, QWidget* parent = nullptr);
    ~GameWindow();

private slots:
    void onQuestionReceived(uint32_t questionId, const QString& content,
                            const QStringList& options, uint32_t timeLimitSec);
    void onRoundResult(uint8_t correctOption, uint8_t playerCount,
                       const QVector<PlayerRoundResult>& results);
    void onPlayerEliminated(uint32_t userId);
    void onGameOver(uint8_t rankingCount, const QVector<PlayerFinalResult>& rankings, 
                   uint8_t gameEndReason, uint8_t winnerCount);
    void onGamePaused();
    void onGameResumed();
    void onGameTerminated(TerminationReason reason);
    void onConnectionError(const QString& error);

    void onOptionAClicked();
    void onOptionBClicked();
    void onOptionCClicked();
    void onOptionDClicked();
    void onQuestionTick();
    void onReturnedToRoom();
    void onPauseGameClicked();
    void onResumeGameClicked();

private:
    void setupUiTheme();
    void bindSignals();
    void startQuestion(uint32_t questionId, const QString& content,
                       const QStringList& options, uint32_t timeLimitSec);
    void setButtonsEnabled(bool enabled);
    void markSelection(int optionIndex);
    void highlightCorrect(uint8_t correctOption);
    void resetOptionStyles();
    void startQuestionTimer(uint32_t seconds);
    void stopQuestionTimer();
    void updateTimerUi();
    void updateScoreboard();
    QString formatStatus(const ClientPlayer& p) const;
    void applyRoundResults(const QVector<PlayerRoundResult>& results);
    void setFeedback(const QString& text, bool warn = false);
    void handleAnswerSelection(int optionIndex);

    Ui::GameWindow* ui;
    NetworkManager* networkManager;
    SessionState* sessionState;

    GameMode gameMode;
    uint32_t roomId;
    uint32_t hostUserId;

    QVector<ClientPlayer> players;

    QTimer* questionTimer;
    QElapsedTimer answerElapsed;
    int timeRemainingSec;
    uint32_t currentQuestionId;
    uint8_t questionNumber;
    bool questionActive;
    bool answerSent;
    bool paused;

signals:
    void returnedToRoom();
};
