#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QMap>
#include "protocol.h"
#include "../models/GameState.h"

namespace Ui {
class ReplayWindow;
}

class NetworkManager;

class ReplayWindow : public QMainWindow {
    Q_OBJECT

public:
    ReplayWindow(uint32_t sessionId, QWidget* parent = nullptr);
    ~ReplayWindow();

private slots:
    void onReplayDataReceived(StatusCode status, uint32_t sessionId, GameMode mode, 
                             const QVector<ReplayEvent>& events);
    void onReplayTick();
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onNextEventClicked();
    void onPrevEventClicked();
    void onConnectionError(const QString& error);

private:
    void setupUi();
    void loadReplayData();
    void playReplay();
    void pauseReplay();
    void stopReplay();
    void displayCurrentEvent();
    void updateEventList();
    void buildEventLog();

    Ui::ReplayWindow* ui;
    NetworkManager* networkManager;

    uint32_t sessionId;
    GameMode gameMode;
    QVector<ReplayEvent> events;
    
    int currentEventIndex;
    bool isPlaying;
    QTimer* playbackTimer;
    
    struct QuestionData {
        uint32_t questionId;
        QStringList options;
        QMap<uint32_t, uint8_t> playerAnswers;
        QMap<uint32_t, uint32_t> playerResponseTimes;
    };
    
    QMap<uint32_t, QuestionData> questions;
};
