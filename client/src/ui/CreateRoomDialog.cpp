#include "CreateRoomDialog.h"
#include "ui_CreateRoomDialog.h"
#include "../network/NetworkManager.h"
#include <QMessageBox>

CreateRoomDialog::CreateRoomDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateRoomDialog)
    , networkManager(NetworkManager::instance())
{
    ui->setupUi(this);
    setupConnections();
}

CreateRoomDialog::~CreateRoomDialog() {
    delete ui;
}

void CreateRoomDialog::setupConnections() {
    connect(ui->btnCreate, &QPushButton::clicked, this, &CreateRoomDialog::onCreateClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &CreateRoomDialog::onCancelClicked);

    connect(networkManager, &NetworkManager::createRoomResponse,
            this, &CreateRoomDialog::onCreateRoomResponse);
    connect(networkManager, &NetworkManager::networkError,
            this, &CreateRoomDialog::onNetworkError);
}

void CreateRoomDialog::onCreateClicked() {
    if (!validateInput()) {
        return;
    }

    QString roomName = ui->edtRoomName->text().trimmed();
    GameMode gameMode = (GameMode)ui->cmbGameMode->currentIndex();
    uint8_t questionCount = ui->spinQuestionCount->value();

    networkManager->sendCreateRoom(roomName, gameMode, questionCount, maxPlayers);
    this->accept();
}

void CreateRoomDialog::onCancelClicked() {
    this->reject();
}

void CreateRoomDialog::onCreateRoomResponse(StatusCode code, const RoomInfo& room_info) {
    if (code == StatusCode::SUCCESS) { // Success
        QMessageBox::information(this, "Success",
            QString("Room '%1' created successfully!").arg(room_info.room_name));
    } else {
        QMessageBox::critical(this, "Error",
            QString("Failed to create room. Error code: %1").arg((uint8_t)code));
    }
}

void CreateRoomDialog::onNetworkError(const QString &error) {
    ui->lblError->setText("Network error: " + error);
    QMessageBox::critical(this, "Network Error", error);
}

bool CreateRoomDialog::validateInput() {
    QString roomName = ui->edtRoomName->text().trimmed();

    if (roomName.isEmpty()) {
        ui->lblError->setText("Room name cannot be empty!");
        return false;
    }

    if (roomName.length() > 32) {
        ui->lblError->setText("Room name cannot exceed 32 characters!");
        return false;
    }

    if (ui->spinQuestionCount->value() < 5 || ui->spinQuestionCount->value() > 20) {
        ui->lblError->setText("Question count must be between 5 and 20!");
        return false;
    }

    ui->lblError->setText("");
    return true;
}
