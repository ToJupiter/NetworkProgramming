#include "ReplayWindow.h"
#include "ui_ReplayWindow.h"
#include "../network/NetworkManager.h"
#include <QMessageBox>

ReplayWindow::ReplayWindow(uint32_t sessionId, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ReplayWindow)
    , networkManager(&NetworkManager::instance())
    , sessionId(sessionId)
    , gameMode(GameMode::ELIMINATION)
    , currentIndex(-1)
    , isPlaying(false)
    , playbackTimer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle(QString("Replay - Session %1").arg(sessionId));
    
    ui->btnOptionA->setStyleSheet("background:#e74c3c; color:white; font-size:14px; font-weight:bold;");
    ui->btnOptionB->setStyleSheet("background:#2980b9; color:white; font-size:14px; font-weight:bold;");
    ui->btnOptionC->setStyleSheet("background:#f1c40f; color:black; font-size:14px; font-weight:bold;");
    ui->btnOptionD->setStyleSheet("background:#27ae60; color:white; font-size:14px; font-weight:bold;");
    
    ui->btnOptionA->setEnabled(false);
    ui->btnOptionB->setEnabled(false);
    ui->btnOptionC->setEnabled(false);
    ui->btnOptionD->setEnabled(false);
    
    connect(playbackTimer, &QTimer::timeout, this, &ReplayWindow::onPlaybackTick);
    connect(networkManager, &NetworkManager::replayDataResponse, this, &ReplayWindow::onReplayDataReceived);
    connect(networkManager, &NetworkManager::connectionError, this, &ReplayWindow::onConnectionError);
    
    connect(ui->btnPlay, &QPushButton::clicked, this, &ReplayWindow::onPlayClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &ReplayWindow::onPauseClicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &ReplayWindow::onNextClicked);
    connect(ui->btnPrev, &QPushButton::clicked, this, &ReplayWindow::onPrevClicked);
    connect(ui->btnClose, &QPushButton::clicked, this, &ReplayWindow::onCloseClicked);
    
    ui->lblStatus->setText("Loading replay data...");
    ui->lblQuestion->setText("");
    ui->lblQuestionInfo->setText("");
    
    networkManager->sendGetReplay(sessionId);
}

ReplayWindow::~ReplayWindow() {
    playbackTimer->stop();
    delete ui;
}

void ReplayWindow::onReplayDataReceived(StatusCode status, uint32_t responseSessionId, 
                                        GameMode mode, const QVector<ReplayEvent>& eventList) {
    if (responseSessionId != sessionId) return;
    
    if (status != StatusCode::SUCCESS) {
        ui->lblStatus->setText("Failed to load replay data");
        QMessageBox::critical(this, "Error", "Failed to load replay data");
        return;
    }
    
    if (eventList.isEmpty()) {
        ui->lblStatus->setText("No replay events found for this session");
        return;
    }
    
    gameMode = mode;
    events = eventList;
    currentIndex = 0;
    
    QString modeStr = (gameMode == GameMode::ELIMINATION) ? "Elimination" : "Scoring";
    ui->lblStatus->setText(QString("Loaded %1 questions - Mode: %2").arg(events.size()).arg(modeStr));
    
    displayQuestion(0);
    updateNavigationButtons();
}

void ReplayWindow::displayQuestion(int index) {
    if (index < 0 || index >= events.size()) return;
    
    currentIndex = index;
    const ReplayEvent& event = events[index];
    
    ui->lblQuestionInfo->setText(QString("Question %1 of %2").arg(index + 1).arg(events.size()));
    ui->lblQuestion->setText(QString::fromUtf8(event.question_content));
    
    ui->btnOptionA->setText(QString("A) %1").arg(QString::fromUtf8(event.options[0])));
    ui->btnOptionB->setText(QString("B) %1").arg(QString::fromUtf8(event.options[1])));
    ui->btnOptionC->setText(QString("C) %1").arg(QString::fromUtf8(event.options[2])));
    ui->btnOptionD->setText(QString("D) %1").arg(QString::fromUtf8(event.options[3])));
    
    resetOptionStyles();
    highlightSelectedOption(event.selected_option);
    
    QString responseInfo = QString("Your answer: Option %1 | Response time: %2 ms")
        .arg(QChar('A' + event.selected_option - 1))
        .arg(event.response_time_ms);
    ui->lblResponseInfo->setText(responseInfo);
    
    updateNavigationButtons();
}

void ReplayWindow::highlightSelectedOption(uint8_t option) {
    QString selectedStyle = "background:#9b59b6; color:white; font-size:14px; font-weight:bold; border:3px solid #2c3e50;";
    
    switch (option) {
        case 1: ui->btnOptionA->setStyleSheet(selectedStyle); break;
        case 2: ui->btnOptionB->setStyleSheet(selectedStyle); break;
        case 3: ui->btnOptionC->setStyleSheet(selectedStyle); break;
        case 4: ui->btnOptionD->setStyleSheet(selectedStyle); break;
    }
}

void ReplayWindow::resetOptionStyles() {
    ui->btnOptionA->setStyleSheet("background:#e74c3c; color:white; font-size:14px; font-weight:bold;");
    ui->btnOptionB->setStyleSheet("background:#2980b9; color:white; font-size:14px; font-weight:bold;");
    ui->btnOptionC->setStyleSheet("background:#f1c40f; color:black; font-size:14px; font-weight:bold;");
    ui->btnOptionD->setStyleSheet("background:#27ae60; color:white; font-size:14px; font-weight:bold;");
}

void ReplayWindow::updateNavigationButtons() {
    ui->btnPrev->setEnabled(currentIndex > 0);
    ui->btnNext->setEnabled(currentIndex < events.size() - 1);
}

void ReplayWindow::setPlaybackState(bool playing) {
    isPlaying = playing;
    ui->btnPlay->setEnabled(!playing);
    ui->btnPause->setEnabled(playing);
}

void ReplayWindow::onPlayClicked() {
    if (events.isEmpty()) return;
    setPlaybackState(true);
    playbackTimer->start(2000);
}

void ReplayWindow::onPauseClicked() {
    playbackTimer->stop();
    setPlaybackState(false);
}

void ReplayWindow::onNextClicked() {
    if (currentIndex < events.size() - 1) {
        displayQuestion(currentIndex + 1);
    }
}

void ReplayWindow::onPrevClicked() {
    if (currentIndex > 0) {
        displayQuestion(currentIndex - 1);
    }
}

void ReplayWindow::onCloseClicked() {
    playbackTimer->stop();
    close();
}

void ReplayWindow::onPlaybackTick() {
    if (currentIndex < events.size() - 1) {
        displayQuestion(currentIndex + 1);
    } else {
        playbackTimer->stop();
        setPlaybackState(false);
    }
}

void ReplayWindow::onConnectionError(const QString& error) {
    ui->lblStatus->setText("Connection error");
    QMessageBox::critical(this, "Connection Error", error);
}
