/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_top;
    QLabel *lblTitle;
    QSpacerItem *verticalSpacer_2;
    QTabWidget *tabWidget;
    QWidget *tabLogin;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_3;
    QLabel *lblLoginEmail;
    QLineEdit *txtLoginEmail;
    QLabel *lblLoginPassword;
    QLineEdit *txtLoginPassword;
    QSpacerItem *verticalSpacer_4;
    QPushButton *btnLogin;
    QSpacerItem *verticalSpacer_5;
    QWidget *tabRegister;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_6;
    QLabel *lblRegEmail;
    QLineEdit *txtRegEmail;
    QLabel *lblRegDisplayName;
    QLineEdit *txtRegDisplayName;
    QLabel *lblRegPassword;
    QLineEdit *txtRegPassword;
    QLabel *lblRegConfirmPassword;
    QLineEdit *txtRegConfirmPassword;
    QSpacerItem *verticalSpacer_7;
    QPushButton *btnRegister;
    QSpacerItem *verticalSpacer_8;
    QLabel *lblError;
    QSpacerItem *verticalSpacer_bottom;

    void setupUi(QMainWindow *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName(QString::fromUtf8("LoginWindow"));
        LoginWindow->resize(500, 600);
        centralwidget = new QWidget(LoginWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer_top = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_top);

        lblTitle = new QLabel(centralwidget);
        lblTitle->setObjectName(QString::fromUtf8("lblTitle"));
        lblTitle->setAlignment(Qt::AlignCenter);
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);
        lblTitle->setFont(font);

        verticalLayout->addWidget(lblTitle);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabLogin = new QWidget();
        tabLogin->setObjectName(QString::fromUtf8("tabLogin"));
        verticalLayout_2 = new QVBoxLayout(tabLogin);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);

        lblLoginEmail = new QLabel(tabLogin);
        lblLoginEmail->setObjectName(QString::fromUtf8("lblLoginEmail"));

        verticalLayout_2->addWidget(lblLoginEmail);

        txtLoginEmail = new QLineEdit(tabLogin);
        txtLoginEmail->setObjectName(QString::fromUtf8("txtLoginEmail"));

        verticalLayout_2->addWidget(txtLoginEmail);

        lblLoginPassword = new QLabel(tabLogin);
        lblLoginPassword->setObjectName(QString::fromUtf8("lblLoginPassword"));

        verticalLayout_2->addWidget(lblLoginPassword);

        txtLoginPassword = new QLineEdit(tabLogin);
        txtLoginPassword->setObjectName(QString::fromUtf8("txtLoginPassword"));
        txtLoginPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(txtLoginPassword);

        verticalSpacer_4 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_4);

        btnLogin = new QPushButton(tabLogin);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));
        btnLogin->setMinimumHeight(40);

        verticalLayout_2->addWidget(btnLogin);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_5);

        tabWidget->addTab(tabLogin, QString());
        tabRegister = new QWidget();
        tabRegister->setObjectName(QString::fromUtf8("tabRegister"));
        verticalLayout_3 = new QVBoxLayout(tabRegister);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer_6 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_6);

        lblRegEmail = new QLabel(tabRegister);
        lblRegEmail->setObjectName(QString::fromUtf8("lblRegEmail"));

        verticalLayout_3->addWidget(lblRegEmail);

        txtRegEmail = new QLineEdit(tabRegister);
        txtRegEmail->setObjectName(QString::fromUtf8("txtRegEmail"));

        verticalLayout_3->addWidget(txtRegEmail);

        lblRegDisplayName = new QLabel(tabRegister);
        lblRegDisplayName->setObjectName(QString::fromUtf8("lblRegDisplayName"));

        verticalLayout_3->addWidget(lblRegDisplayName);

        txtRegDisplayName = new QLineEdit(tabRegister);
        txtRegDisplayName->setObjectName(QString::fromUtf8("txtRegDisplayName"));
        txtRegDisplayName->setMaxLength(32);

        verticalLayout_3->addWidget(txtRegDisplayName);

        lblRegPassword = new QLabel(tabRegister);
        lblRegPassword->setObjectName(QString::fromUtf8("lblRegPassword"));

        verticalLayout_3->addWidget(lblRegPassword);

        txtRegPassword = new QLineEdit(tabRegister);
        txtRegPassword->setObjectName(QString::fromUtf8("txtRegPassword"));
        txtRegPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(txtRegPassword);

        lblRegConfirmPassword = new QLabel(tabRegister);
        lblRegConfirmPassword->setObjectName(QString::fromUtf8("lblRegConfirmPassword"));

        verticalLayout_3->addWidget(lblRegConfirmPassword);

        txtRegConfirmPassword = new QLineEdit(tabRegister);
        txtRegConfirmPassword->setObjectName(QString::fromUtf8("txtRegConfirmPassword"));
        txtRegConfirmPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(txtRegConfirmPassword);

        verticalSpacer_7 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_7);

        btnRegister = new QPushButton(tabRegister);
        btnRegister->setObjectName(QString::fromUtf8("btnRegister"));
        btnRegister->setMinimumHeight(40);

        verticalLayout_3->addWidget(btnRegister);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_8);

        tabWidget->addTab(tabRegister, QString());

        verticalLayout->addWidget(tabWidget);

        lblError = new QLabel(centralwidget);
        lblError->setObjectName(QString::fromUtf8("lblError"));
        lblError->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lblError);

        verticalSpacer_bottom = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_bottom);

        LoginWindow->setCentralWidget(centralwidget);

        retranslateUi(LoginWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QMainWindow *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "Quiz Game - Login", nullptr));
        lblTitle->setText(QCoreApplication::translate("LoginWindow", "MULTIPLAYER QUIZ GAME", nullptr));
        lblLoginEmail->setText(QCoreApplication::translate("LoginWindow", "Email:", nullptr));
        txtLoginEmail->setPlaceholderText(QCoreApplication::translate("LoginWindow", "your.email@example.com", nullptr));
        lblLoginPassword->setText(QCoreApplication::translate("LoginWindow", "Password:", nullptr));
        txtLoginPassword->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Enter password", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLogin), QCoreApplication::translate("LoginWindow", "Login", nullptr));
        lblRegEmail->setText(QCoreApplication::translate("LoginWindow", "Email:", nullptr));
        txtRegEmail->setPlaceholderText(QCoreApplication::translate("LoginWindow", "your.email@example.com", nullptr));
        lblRegDisplayName->setText(QCoreApplication::translate("LoginWindow", "Display Name:", nullptr));
        txtRegDisplayName->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Your in-game name", nullptr));
        lblRegPassword->setText(QCoreApplication::translate("LoginWindow", "Password:", nullptr));
        txtRegPassword->setPlaceholderText(QCoreApplication::translate("LoginWindow", "At least 6 characters", nullptr));
        lblRegConfirmPassword->setText(QCoreApplication::translate("LoginWindow", "Confirm Password:", nullptr));
        txtRegConfirmPassword->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Re-enter password", nullptr));
        btnRegister->setText(QCoreApplication::translate("LoginWindow", "Register", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRegister), QCoreApplication::translate("LoginWindow", "Register", nullptr));
        lblError->setText(QString());
        lblError->setStyleSheet(QCoreApplication::translate("LoginWindow", "color: red;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
