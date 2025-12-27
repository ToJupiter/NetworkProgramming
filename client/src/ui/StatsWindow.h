#pragma once

#include <QDialog>
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StatsWindow; }
QT_END_NAMESPACE

class StatsWindow : public QDialog {
    Q_OBJECT

public:
    explicit StatsWindow(QWidget *parent = nullptr);
    ~StatsWindow();

    void setStats(const UserStatsResponse &stats);
    void showLoading();

private:
    Ui::StatsWindow *ui;
};
