#include "LoginWindow.h"
#include "LobbyWindow.h"
#include "ui_LoginWindow.h"
#include "../network/NetworkManager.h"
#include "../models/SessionState.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QShowEvent>
#include <QShowEvent>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , lobbyWindow(nullptr)
{
    ui->setupUi(this);
    
    // Connect UI signals
    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    
    // Connect network signals
    auto& network = NetworkManager::instance();
    connect(&network, &NetworkManager::connected, this, &LoginWindow::onConnected);
    connect(&network, &NetworkManager::disconnected, this, &LoginWindow::onDisconnected);
    connect(&network, &NetworkManager::connectionError, this, &LoginWindow::onConnectionError);
    connect(&network, &NetworkManager::registerResponse, this, &LoginWindow::onRegisterResponse);
    connect(&network, &NetworkManager::loginResponse, this, &LoginWindow::onLoginResponse);
    
    // Connect to server on startup
    connectToServer();
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::connectToServer() {
    auto& network = NetworkManager::instance();
    
    // TODO: Make this configurable via settings dialog
    QString host = "127.0.0.1";
    quint16 port = 10002;
    
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
    ui->lblError->setText("Connecting to server...");
    
    if (!network.connectToServer(host, port)) {
        showError("Failed to connect to server!");
        ui->btnLogin->setEnabled(true);
        ui->btnRegister->setEnabled(true);
    }
}

void LoginWindow::onLoginClicked() {
    if (!validateLoginInput()) {
        return;
    }
    
    QString email = ui->txtLoginEmail->text().trimmed();
    QString password = ui->txtLoginPassword->text();
    
    ui->btnLogin->setEnabled(false);
    clearError();
    
    NetworkManager::instance().sendLogin(email, password);
}

void LoginWindow::onRegisterClicked() {
    if (!validateRegisterInput()) {
        return;
    }
    
    QString email = ui->txtRegEmail->text().trimmed();
    QString displayName = ui->txtRegDisplayName->text().trimmed();
    QString password = ui->txtRegPassword->text();
    
    ui->btnRegister->setEnabled(false);
    clearError();
    
    NetworkManager::instance().sendRegister(email, displayName, password);
}

bool LoginWindow::validateLoginInput() {
    QString email = ui->txtLoginEmail->text().trimmed();
    QString password = ui->txtLoginPassword->text();
    
    if (email.isEmpty()) {
        showError("Please enter your email");
        return false;
    }
    
    if (password.isEmpty()) {
        showError("Please enter your password");
        return false;
    }
    
    return true;
}

bool LoginWindow::validateRegisterInput() {
    QString email = ui->txtRegEmail->text().trimmed();
    QString displayName = ui->txtRegDisplayName->text().trimmed();
    QString password = ui->txtRegPassword->text();
    QString confirmPassword = ui->txtRegConfirmPassword->text();
    
    // Validate email
    if (email.isEmpty()) {
        showError("Please enter your email");
        return false;
    }
    
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        showError("Please enter a valid email address");
        return false;
    }
    
    // Validate display name
    if (displayName.isEmpty()) {
        showError("Please enter a display name");
        return false;
    }
    
    if (displayName.length() > 32) {
        showError("Display name must be 32 characters or less");
        return false;
    }
    
    // Validate password
    if (password.isEmpty()) {
        showError("Please enter a password");
        return false;
    }
    
    if (password.length() < 6) {
        showError("Password must be at least 6 characters");
        return false;
    }
    
    if (password != confirmPassword) {
        showError("Passwords do not match");
        return false;
    }
    
    return true;
}

void LoginWindow::onConnected() {
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
    clearError();
}

void LoginWindow::onDisconnected() {
    // Only show error if LoginWindow is visible (user is on login screen)
    // If user is in Lobby and logs out, LobbyWindow handles the transition
    if (this->isVisible()) {
        showError("Disconnected from server");
        ui->btnLogin->setEnabled(false);
        ui->btnRegister->setEnabled(false);
    }
}

void LoginWindow::onConnectionError(const QString& error) {
    showError("Connection error: " + error);
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
}

void LoginWindow::onRegisterResponse(StatusCode code) {
    ui->btnRegister->setEnabled(true);
    
    if (code == StatusCode::SUCCESS) {
        QMessageBox::information(this, "Success", "Registration successful! Please login.");
        ui->tabWidget->setCurrentIndex(0); // Switch to login tab
        clearError();
    } else if (code == StatusCode::EMAIL_EXISTS) {
        showError("Email already exists!");
    } else {
        showError("Registration failed. Please try again.");
    }
}

void LoginWindow::onLoginResponse(StatusCode code, uint32_t userId, const QString& displayName) {
    ui->btnLogin->setEnabled(true);
    
    if (code == StatusCode::SUCCESS) {
        // Store session state
        SessionState::instance().setAuthenticated(true);
        SessionState::instance().setUserId(userId);
        SessionState::instance().setDisplayName(displayName);
        SessionState::instance().setEmail(ui->txtLoginEmail->text().trimmed());

        clearError();
        
        // Hide LoginWindow and show LobbyWindow
        this->hide();
        
        // Create and show LobbyWindow
        if (!lobbyWindow) {
            lobbyWindow = new LobbyWindow();
        }
        lobbyWindow->show();
    } else if (code == StatusCode::INVALID_CREDENTIALS) {
        showError("Invalid email or password!");
    } else {
        showError("Login failed. Please try again.");
    }
}

void LoginWindow::showError(const QString& message) {
    ui->lblError->setText(message);
}

void LoginWindow::clearError() {
    ui->lblError->setText("");
}

void LoginWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    
    // If we're being shown again (e.g., after logout), check connection
    auto& network = NetworkManager::instance();
    if (!network.isConnected()) {
        // Reconnect to server
        connectToServer();
    } else {
        // Already connected, just clear error and enable buttons
        clearError();
        ui->btnLogin->setEnabled(true);
        ui->btnRegister->setEnabled(true);
    }
}
