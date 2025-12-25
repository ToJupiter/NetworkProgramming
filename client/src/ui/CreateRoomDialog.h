#ifndef CREATEROCOMDIALOG_H
#define CREATEROCOMDIALOG_H

#include <QDialog>
#include "protocol.h"

class NetworkManager;

namespace Ui {
class CreateRoomDialog;
}

class CreateRoomDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateRoomDialog(QWidget *parent = nullptr);
    ~CreateRoomDialog();

private slots:
    void onCreateClicked();
    void onCancelClicked();
    void onCreateRoomResponse(StatusCode code, const RoomInfo& room_info);
    void onConnectionError(const QString &error);

private:
    void setupConnections();
    bool validateInput();

    Ui::CreateRoomDialog *ui;
    NetworkManager *networkManager;
};

#endif // CREATEROCOMDIALOG_H
