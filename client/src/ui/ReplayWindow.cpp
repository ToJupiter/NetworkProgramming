#include "ReplayWindow.h"
#include "ui_ReplayWindow.h"
#include "../network/NetworkManager.h"
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>

namespace {
QString formatTime(uint32_t ms) {
    uint32_t sec = ms / 1000;
    uint32_t msec = ms % 1000;
    return QString("%1.%2s").arg(sec).arg(msec, 3, 10, QChar('0'));
}
}

ReplayWindow::ReplayWindow(uint32_t sessionId, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ReplayWindow)
    , networkManager(&NetworkManager::instance())
    , sessionId(sessionId)
    , gameMode(GameMode::ELIMINATION)
    , currentEventIndex(0)
    , isPlaying(false)
    , playbackTimer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle(QString("Replay - Session %1").arg(sessionId));
    
    connect(playbackTimer, &QTimer::timeout, this, &ReplayWindow::onReplayTick);
    connect(networkManager, &NetworkManager::replayDataResponse,
            this, &ReplayWindow::onReplayDataReceived);
    connect(networkManager, &NetworkManager::connectionError,
            this, &ReplayWindow::onConnectionError);
    
    ui->btnPlay->setStyleSheet("background:#27ae60; color:white; font-weight:bold;");
    ui->btnPause->setStyleSheet("background:#f39c12; color:white; font-weight:bold;");
    ui->btnStop->setStyleSheet("background:#c0392b; color:white; font-weight:bold;");
    ui->btnNext->setStyleSheet("background:#3498db; color:white; font-weight:bold;");
    ui->btnPrev->setStyleSheet("background:#9b59b6; color:white; font-weight:bold;");
    
    connect(ui->btnPlay, &QPushButton::clicked, this, &ReplayWindow::onPlayClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &ReplayWindow::onPauseClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &ReplayWindow::onStopClicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &ReplayWindow::onNextEventClicked);
    connect(ui->btnPrev, &QPushButton::clicked, this, &ReplayWindow::onPrevEventClicked);
    
    ui->tblAnswers->setColumnCount(3);
    ui->tblAnswers->setHorizontalHeaderLabels({"Player", "Answer", "Response Time"});
    ui->tblAnswers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblAnswers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    ui->lblEventInfo->setText("Waiting for replay data...");
    
    loadReplayData();
}

ReplayWindow::~ReplayWindow() {
    delete ui;
}

void ReplayWindow::loadReplayData() {
    networkManager->sendGetReplay(sessionId);
}

void ReplayWindow::onReplayDataReceived(StatusCode status, uint32_t responseSessionId, 
                                        GameMode mode, const QVector<ReplayEvent>& eventList) {
    if (status != StatusCode::SUCCESS || responseSessionId != sessionId) {
        QMessageBox::critical(this, "Replay Error", "Failed to load replay data");
        return;
    }
    
    gameMode = mode;
    events = eventList;
    
    buildEventLog();
    updateEventList();
    
    if (!events.isEmpty()) {
        currentEventIndex = 0;
        displayCurrentEvent();
    }
}

void ReplayWindow::buildEventLog() {
    questions.clear();
    
    for (const auto& event : events) {
        QuestionData& qdata = questions[event.question_id];
        if (qdata.options.isEmpty()) {
            qdata.questionId = event.question_id;
        }
        
        qdata.playerAnswers[event.user_id] = event.selected_option;
        qdata.playerResponseTimes[event.user_id] = event.response_time_ms;
    }
}

void ReplayWindow::updateEventList() {
    ui->lstEvents->clear();
    
    for (int i = 0; i < events.size(); ++i) {
        const auto& event = events[i];
        QString text = QString("Event %1: Q%2 at %3ms")
            .arg(i + 1)
            .arg(event.question_id)
            .arg(event.timestamp_ms);
        ui->lstEvents->addItem(text);
    }
}

void ReplayWindow::displayCurrentEvent() {
    if (currentEventIndex < 0 || currentEventIndex >= events.size()) {
        ui->lblEventInfo->setText("No events to display");
        return;
    }
    
    const auto& event = events[currentEventIndex];
    
    QString difficultyStr;
    if (event.difficulty == 1) difficultyStr = "Easy";
    else if (event.difficulty == 2) difficultyStr = "Medium";
    else if (event.difficulty == 3) difficultyStr = "Hard";
    else difficultyStr = "Unknown";
    
    QString correctOptionStr = QString("Option %1").arg(QChar('A' + event.correct_option - 1));
    QString selectedOptionStr = QString("Option %1").arg(QChar('A' + event.selected_option - 1));
    bool isCorrect = (event.selected_option == event.correct_option);
    
    ui->lblEventInfo->setText(QString("Event %1 / %2 - %3 (%4) - Answered: %5 (Correct: %6) - %7ms")
        .arg(currentEventIndex + 1)
        .arg(events.size())
        .arg(QString::fromUtf8(event.question_content, strlen(event.question_content)))
        .arg(difficultyStr)
        .arg(selectedOptionStr)
        .arg(isCorrect ? "✓" : "✗")
        .arg(event.response_time_ms));
    
    ui->lstEvents->setCurrentRow(currentEventIndex);
    
    const auto& qdata = questions[event.question_id];
    ui->tblAnswers->setRowCount(0);
    
    int row = 0;
    for (auto it = qdata.playerAnswers.begin(); it != qdata.playerAnswers.end(); ++it) {
        uint32_t userId = it.key();
        uint8_t answer = it.value();
        uint32_t responseTime = qdata.playerResponseTimes.value(userId, 0);
        
        ui->tblAnswers->insertRow(row);
        
        auto* userItem = new QTableWidgetItem(QString::number(userId));
        auto* ansItem = new QTableWidgetItem(QString("Option %1").arg(QChar('A' + answer - 1)));
        auto* timeItem = new QTableWidgetItem(formatTime(responseTime));
        
        userItem->setFlags(userItem->flags() & ~Qt::ItemIsEditable);
        ansItem->setFlags(ansItem->flags() & ~Qt::ItemIsEditable);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        
        ui->tblAnswers->setItem(row, 0, userItem);
        ui->tblAnswers->setItem(row, 1, ansItem);
        ui->tblAnswers->setItem(row, 2, timeItem);
        
        ++row;
    }
}

void ReplayWindow::onPlayClicked() {
    if (!isPlaying) {
        playReplay();
    }
}

void ReplayWindow::onPauseClicked() {
    if (isPlaying) {
        pauseReplay();
    }
}

void ReplayWindow::onStopClicked() {
    stopReplay();
}

void ReplayWindow::onNextEventClicked() {
    if (currentEventIndex < events.size() - 1) {
        currentEventIndex++;
        displayCurrentEvent();
    }
}

void ReplayWindow::onPrevEventClicked() {
    if (currentEventIndex > 0) {
        currentEventIndex--;
        displayCurrentEvent();
    }
}

void ReplayWindow::playReplay() {
    isPlaying = true;
    ui->btnPlay->setEnabled(false);
    ui->btnPause->setEnabled(true);
    playbackTimer->start(500);
}

void ReplayWindow::pauseReplay() {
    isPlaying = false;
    playbackTimer->stop();
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
}

void ReplayWindow::stopReplay() {
    isPlaying = false;
    playbackTimer->stop();
    currentEventIndex = 0;
    displayCurrentEvent();
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
}

void ReplayWindow::onReplayTick() {
    if (isPlaying && currentEventIndex < events.size() - 1) {
        currentEventIndex++;
        displayCurrentEvent();
    } else if (currentEventIndex >= events.size() - 1) {
        pauseReplay();
    }
}

void ReplayWindow::onConnectionError(const QString& error) {
    QMessageBox::critical(this, "Connection Error", error);
}
