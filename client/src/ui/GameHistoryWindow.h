#pragma once

#include <QMainWindow>
#include <QVector>
#include "protocol.h"

namespace Ui {
class GameHistoryWindow;
}

class NetworkManager;

class GameHistoryWindow : public QMainWindow {
    Q_OBJECT

public:
    GameHistoryWindow(QWidget* parent = nullptr);
    ~GameHistoryWindow();

private slots:
    void onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries);
    void onConnectionError(const QString& error);

private:
    void loadHistory();
    void displayHistory(const QVector<GameHistoryEntry>& entries);

    Ui::GameHistoryWindow* ui;
    NetworkManager* networkManager;
};
