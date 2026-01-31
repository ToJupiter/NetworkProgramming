#pragma once

#include <QMainWindow>
#include <QVector>
#include "protocol.h"

namespace Ui {
class GameResultsWindow;
}

struct PlayerResult {
    uint32_t user_id;
    QString display_name;
    uint32_t final_score;
    uint32_t final_rank;
    uint32_t correct_answers;
    uint32_t avg_response_time_ms;
    bool is_winner;
};

class GameResultsWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GameResultsWindow(const QVector<PlayerResult>& results, 
                              GameEndReason endReason,
                              uint8_t winnerCount,
                              QWidget *parent = nullptr);
    ~GameResultsWindow();

signals:
    void leaveRoomRequested();
    void stayInRoomRequested();

private slots:
    void onLeaveRoomClicked();
    void onStayInRoomClicked();

private:
    Ui::GameResultsWindow *ui;
    QVector<PlayerResult> playerResults;
    GameEndReason gameEndReason;
    uint8_t numWinners;
    
    void setupUI();
    void displayResults();
    QString getEndReasonText() const;
};
