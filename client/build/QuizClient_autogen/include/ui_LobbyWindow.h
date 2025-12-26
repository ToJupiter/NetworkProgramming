/********************************************************************************
** Form generated from reading UI file 'LobbyWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOBBYWINDOW_H
#define UI_LOBBYWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LobbyWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_top;
    QLabel *lblUserInfo;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnRefresh;
    QPushButton *btnCreateRoom;
    QPushButton *btnStats;
    QPushButton *btnLogout;
    QLabel *lblRoomListTitle;
    QTableWidget *tblRooms;
    QLabel *lblError;
    QLabel *lblStatus;

    void setupUi(QMainWindow *LobbyWindow)
    {
        if (LobbyWindow->objectName().isEmpty())
            LobbyWindow->setObjectName(QString::fromUtf8("LobbyWindow"));
        LobbyWindow->resize(800, 600);
        centralwidget = new QWidget(LobbyWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_top = new QHBoxLayout();
        horizontalLayout_top->setObjectName(QString::fromUtf8("horizontalLayout_top"));
        lblUserInfo = new QLabel(centralwidget);
        lblUserInfo->setObjectName(QString::fromUtf8("lblUserInfo"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        lblUserInfo->setFont(font);

        horizontalLayout_top->addWidget(lblUserInfo);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_top->addItem(horizontalSpacer);

        btnRefresh = new QPushButton(centralwidget);
        btnRefresh->setObjectName(QString::fromUtf8("btnRefresh"));
        btnRefresh->setMinimumWidth(80);

        horizontalLayout_top->addWidget(btnRefresh);

        btnCreateRoom = new QPushButton(centralwidget);
        btnCreateRoom->setObjectName(QString::fromUtf8("btnCreateRoom"));
        btnCreateRoom->setMinimumWidth(100);

        horizontalLayout_top->addWidget(btnCreateRoom);

        btnStats = new QPushButton(centralwidget);
        btnStats->setObjectName(QString::fromUtf8("btnStats"));
        btnStats->setMinimumWidth(80);

        horizontalLayout_top->addWidget(btnStats);

        btnLogout = new QPushButton(centralwidget);
        btnLogout->setObjectName(QString::fromUtf8("btnLogout"));
        btnLogout->setMinimumWidth(80);

        horizontalLayout_top->addWidget(btnLogout);


        verticalLayout->addLayout(horizontalLayout_top);

        lblRoomListTitle = new QLabel(centralwidget);
        lblRoomListTitle->setObjectName(QString::fromUtf8("lblRoomListTitle"));
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setWeight(75);
        lblRoomListTitle->setFont(font1);

        verticalLayout->addWidget(lblRoomListTitle);

        tblRooms = new QTableWidget(centralwidget);
        if (tblRooms->columnCount() < 6)
            tblRooms->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tblRooms->setObjectName(QString::fromUtf8("tblRooms"));
        tblRooms->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblRooms->setSelectionMode(QAbstractItemView::SingleSelection);
        tblRooms->setColumnCount(6);
        tblRooms->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tblRooms);

        lblError = new QLabel(centralwidget);
        lblError->setObjectName(QString::fromUtf8("lblError"));

        verticalLayout->addWidget(lblError);

        lblStatus = new QLabel(centralwidget);
        lblStatus->setObjectName(QString::fromUtf8("lblStatus"));
        lblStatus->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lblStatus);

        LobbyWindow->setCentralWidget(centralwidget);

        retranslateUi(LobbyWindow);

        QMetaObject::connectSlotsByName(LobbyWindow);
    } // setupUi

    void retranslateUi(QMainWindow *LobbyWindow)
    {
        LobbyWindow->setWindowTitle(QCoreApplication::translate("LobbyWindow", "Quiz Game - Lobby", nullptr));
        lblUserInfo->setText(QCoreApplication::translate("LobbyWindow", "Welcome, Player", nullptr));
        btnRefresh->setText(QCoreApplication::translate("LobbyWindow", "Refresh", nullptr));
        btnCreateRoom->setText(QCoreApplication::translate("LobbyWindow", "Create Room", nullptr));
        btnStats->setText(QCoreApplication::translate("LobbyWindow", "Stats", nullptr));
        btnLogout->setText(QCoreApplication::translate("LobbyWindow", "Logout", nullptr));
        lblRoomListTitle->setText(QCoreApplication::translate("LobbyWindow", "Available Rooms", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tblRooms->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("LobbyWindow", "Room ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tblRooms->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("LobbyWindow", "Room Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tblRooms->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("LobbyWindow", "Players", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tblRooms->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("LobbyWindow", "Mode", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tblRooms->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("LobbyWindow", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tblRooms->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("LobbyWindow", "Action", nullptr));
        lblError->setText(QString());
        lblError->setStyleSheet(QCoreApplication::translate("LobbyWindow", "color: red;", nullptr));
        lblStatus->setText(QCoreApplication::translate("LobbyWindow", "Loading rooms...", nullptr));
        lblStatus->setStyleSheet(QCoreApplication::translate("LobbyWindow", "color: gray;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LobbyWindow: public Ui_LobbyWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOBBYWINDOW_H
