#pragma once

#include <QMainWindow>
#include <QVector>
#include "protocol.h"

namespace Ui {
class MainMenuWindow;
}

class NetworkManager;

class MainMenuWindow : public QMainWindow {
    Q_OBJECT

public:
    MainMenuWindow(QWidget* parent = nullptr);
    ~MainMenuWindow();

private slots:
    void onStatsReceived(const UserStatsResponse& stats);
    void onPlayClicked();
    void onViewStatsClicked();
    void onReplayClicked();
    void onConnectionError(const QString& error);
    void loadStats();

private:
    void setupUI();
    void displayStats(const UserStatsResponse& stats);

    Ui::MainMenuWindow* ui;
    NetworkManager* networkManager;
    UserStatsResponse currentStats;
};
