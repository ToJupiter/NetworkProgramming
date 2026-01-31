#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include "protocol.h"

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
    void onPlayClicked();
    void onPauseClicked();
    void onNextClicked();
    void onPrevClicked();
    void onCloseClicked();
    void onPlaybackTick();
    void onConnectionError(const QString& error);

private:
    void displayQuestion(int index);
    void highlightSelectedOption(uint8_t option);
    void resetOptionStyles();
    void updateNavigationButtons();
    void setPlaybackState(bool playing);

    Ui::ReplayWindow* ui;
    NetworkManager* networkManager;

    uint32_t sessionId;
    GameMode gameMode;
    QVector<ReplayEvent> events;
    
    int currentIndex;
    bool isPlaying;
    QTimer* playbackTimer;
};
