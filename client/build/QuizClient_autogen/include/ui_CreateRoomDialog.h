/********************************************************************************
** Form generated from reading UI file 'CreateRoomDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEROOMDIALOG_H
#define UI_CREATEROOMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CreateRoomDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_1;
    QLineEdit *edtRoomName;
    QLabel *label_2;
    QComboBox *cmbGameMode;
    QLabel *label_3;
    QSpinBox *spinQuestionCount;
    QLabel *lblError;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnCreate;
    QPushButton *btnCancel;

    void setupUi(QDialog *CreateRoomDialog)
    {
        if (CreateRoomDialog->objectName().isEmpty())
            CreateRoomDialog->setObjectName(QString::fromUtf8("CreateRoomDialog"));
        CreateRoomDialog->resize(400, 220);
        CreateRoomDialog->setModal(true);
        verticalLayout = new QVBoxLayout(CreateRoomDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_1 = new QLabel(CreateRoomDialog);
        label_1->setObjectName(QString::fromUtf8("label_1"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_1->setFont(font);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_1);

        edtRoomName = new QLineEdit(CreateRoomDialog);
        edtRoomName->setObjectName(QString::fromUtf8("edtRoomName"));
        edtRoomName->setMaxLength(32);

        formLayout->setWidget(0, QFormLayout::FieldRole, edtRoomName);

        label_2 = new QLabel(CreateRoomDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        cmbGameMode = new QComboBox(CreateRoomDialog);
        cmbGameMode->addItem(QString());
        cmbGameMode->addItem(QString());
        cmbGameMode->setObjectName(QString::fromUtf8("cmbGameMode"));

        formLayout->setWidget(1, QFormLayout::FieldRole, cmbGameMode);

        label_3 = new QLabel(CreateRoomDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        spinQuestionCount = new QSpinBox(CreateRoomDialog);
        spinQuestionCount->setObjectName(QString::fromUtf8("spinQuestionCount"));
        spinQuestionCount->setMinimum(5);
        spinQuestionCount->setMaximum(20);
        spinQuestionCount->setValue(10);

        formLayout->setWidget(2, QFormLayout::FieldRole, spinQuestionCount);


        verticalLayout->addLayout(formLayout);

        lblError = new QLabel(CreateRoomDialog);
        lblError->setObjectName(QString::fromUtf8("lblError"));

        verticalLayout->addWidget(lblError);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnCreate = new QPushButton(CreateRoomDialog);
        btnCreate->setObjectName(QString::fromUtf8("btnCreate"));
        btnCreate->setMinimumWidth(80);

        horizontalLayout->addWidget(btnCreate);

        btnCancel = new QPushButton(CreateRoomDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        btnCancel->setMinimumWidth(80);

        horizontalLayout->addWidget(btnCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(CreateRoomDialog);

        QMetaObject::connectSlotsByName(CreateRoomDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateRoomDialog)
    {
        CreateRoomDialog->setWindowTitle(QCoreApplication::translate("CreateRoomDialog", "Create New Room", nullptr));
        label_1->setText(QCoreApplication::translate("CreateRoomDialog", "Room Name:", nullptr));
        edtRoomName->setPlaceholderText(QCoreApplication::translate("CreateRoomDialog", "Enter room name (1-32 chars)", nullptr));
        label_2->setText(QCoreApplication::translate("CreateRoomDialog", "Game Mode:", nullptr));
        cmbGameMode->setItemText(0, QCoreApplication::translate("CreateRoomDialog", "Elimination (players eliminated on wrong answer)", nullptr));
        cmbGameMode->setItemText(1, QCoreApplication::translate("CreateRoomDialog", "Scoring (points awarded for correct answers)", nullptr));

        label_3->setText(QCoreApplication::translate("CreateRoomDialog", "Question Count:", nullptr));
        lblError->setText(QString());
        lblError->setStyleSheet(QCoreApplication::translate("CreateRoomDialog", "color: red;", nullptr));
        btnCreate->setText(QCoreApplication::translate("CreateRoomDialog", "Create", nullptr));
        btnCancel->setText(QCoreApplication::translate("CreateRoomDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateRoomDialog: public Ui_CreateRoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEROOMDIALOG_H
