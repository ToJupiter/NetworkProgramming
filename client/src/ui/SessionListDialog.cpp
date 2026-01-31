#include "SessionListDialog.h"
#include "ui_SessionListDialog.h"
#include "../network/NetworkManager.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>

SessionListDialog::SessionListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SessionListDialog)
    , networkManager(&NetworkManager::instance())
    , selectedSessionId(0)
    , selectedRow(-1)
{
    ui->setupUi(this);
    setWindowTitle("Select Game Session to Replay");
    
    // Configure table
    ui->tblSessions->setColumnCount(5);
    ui->tblSessions->setHorizontalHeaderLabels({
        "Session ID", "Date", "Mode", "Score", "Rank"
    });
    ui->tblSessions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblSessions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblSessions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblSessions->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Connect signals
    connect(networkManager, &NetworkManager::gameHistoryResponse,
            this, &SessionListDialog::onGameHistoryReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &SessionListDialog::onConnectionError);
    connect(ui->tblSessions, &QTableWidget::cellDoubleClicked,
            this, &SessionListDialog::onSessionDoubleClicked);
    connect(ui->btnOk, &QPushButton::clicked, this, &SessionListDialog::onOkClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SessionListDialog::onCancelClicked);
    
    loadSessions();
}

SessionListDialog::~SessionListDialog() {
    delete ui;
}

void SessionListDialog::loadSessions() {
    ui->lblStatus->setText("Loading game sessions...");
    ui->tblSessions->setRowCount(0);
    networkManager->sendGetGameHistory();
}

void SessionListDialog::onGameHistoryReceived(StatusCode status, const QVector<GameHistoryEntry>& entries) {
    if (status == StatusCode::SUCCESS) {
        sessions = entries;
        displaySessions(entries);
    } else {
        ui->lblStatus->setText("Failed to load game sessions");
        QMessageBox::warning(this, "Error", "Failed to load game history");
    }
}

void SessionListDialog::displaySessions(const QVector<GameHistoryEntry>& entries) {
    ui->tblSessions->setRowCount(0);
    
    for (const auto& entry : entries) {
        int row = ui->tblSessions->rowCount();
        ui->tblSessions->insertRow(row);
        
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(entry.timestamp_sec);
        QString dateStr = dateTime.toString("yyyy-MM-dd hh:mm");
        
        auto* idItem = new QTableWidgetItem(QString::number(entry.session_id));
        auto* dateItem = new QTableWidgetItem(dateStr);
        auto* modeItem = new QTableWidgetItem(QString::fromLatin1(entry.game_mode));
        auto* scoreItem = new QTableWidgetItem(QString::number(entry.player_score));
        auto* rankItem = new QTableWidgetItem(entry.player_rank > 0 ? QString::number(entry.player_rank) : "—");
        
        ui->tblSessions->setItem(row, 0, idItem);
        ui->tblSessions->setItem(row, 1, dateItem);
        ui->tblSessions->setItem(row, 2, modeItem);
        ui->tblSessions->setItem(row, 3, scoreItem);
        ui->tblSessions->setItem(row, 4, rankItem);
    }
    
    ui->lblStatus->setText(QString("Found %1 game sessions").arg(entries.size()));
}

void SessionListDialog::onSessionDoubleClicked(int row, int /*column*/) {
    if (row >= 0 && row < sessions.size()) {
        selectedSessionId = sessions[row].session_id;
        selectedRow = row;
        accept();
    }
}

void SessionListDialog::onOkClicked() {
    int currentRow = ui->tblSessions->currentRow();
    if (currentRow >= 0 && currentRow < sessions.size()) {
        selectedSessionId = sessions[currentRow].session_id;
        selectedRow = currentRow;
        accept();
    } else {
        QMessageBox::information(this, "No Selection", "Please select a session to replay");
    }
}

void SessionListDialog::onCancelClicked() {
    reject();
}

void SessionListDialog::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}
