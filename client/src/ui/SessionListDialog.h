#pragma once

#include <QDialog>
#include <QVector>
#include "protocol.h"

namespace Ui {
class SessionListDialog;
}

class NetworkManager;

class SessionListDialog : public QDialog {
    Q_OBJECT

public:
    explicit SessionListDialog(QWidget *parent = nullptr);
    ~SessionListDialog();
    
    uint32_t getSelectedSessionId() const { return selectedSessionId; }

private slots:
    void onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries);
    void onConnectionError(const QString& error);
    void onSessionDoubleClicked(int row, int column);
    void onOkClicked();
    void onCancelClicked();

private:
    void loadSessions();
    void displaySessions(const QVector<GameHistoryEntry>& entries);
    
    Ui::SessionListDialog *ui;
    NetworkManager *networkManager;
    QVector<GameHistoryEntry> sessions;
    uint32_t selectedSessionId;
    int selectedRow;
};
