#pragma once

#include <QMainWindow>
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow {
    Q_OBJECT
    
public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    
private slots:
    void onLoginClicked();
    void onRegisterClicked();
    
    void onConnected();
    void onDisconnected();
    void onConnectionError(const QString& error);
    void onRegisterResponse(StatusCode code);
    void onLoginResponse(StatusCode code, uint32_t userId, const QString& displayName);
    
private:
    void connectToServer();
    bool validateLoginInput();
    bool validateRegisterInput();
    void showError(const QString& message);
    void clearError();
    
    Ui::LoginWindow *ui;
};
