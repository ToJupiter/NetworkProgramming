#include "StatsWindow.h"
#include "ui_StatsWindow.h"
#include <QString>
#include <QtGlobal>

static QString formatPercent(double value) {
    return QString::number(value, 'f', 1) + "%";
}

StatsWindow::StatsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    showLoading();
}

StatsWindow::~StatsWindow() {
    delete ui;
}

void StatsWindow::showLoading() {
    ui->lblMatchesValue->setText("Loading...");
    ui->lblWinsValue->setText("-");
    ui->lblWinRateValue->setText("-");
    ui->lblCorrectValue->setText("-");
    ui->lblIncorrectValue->setText("-");
    ui->lblAccuracyValue->setText("-");
    ui->lblAverageScoreValue->setText("-");
    ui->lblRankedPointsValue->setText("-");
}

void StatsWindow::setStats(const UserStatsResponse &stats) {
    const uint32_t matches = stats.total_matches;
    const uint32_t wins = stats.wins;
    const uint32_t correct = stats.total_correct_answers;
    const uint32_t incorrect = stats.total_incorrect_answers;
    const double totalAnswered = static_cast<double>(correct + incorrect);

    const double winRate = (matches == 0) ? 0.0 : (static_cast<double>(wins) * 100.0 / matches);
    const double accuracy = (totalAnswered == 0.0) ? 0.0 : (static_cast<double>(correct) * 100.0 / totalAnswered);

    ui->lblMatchesValue->setText(QString::number(matches));
    ui->lblWinsValue->setText(QString::number(wins));
    ui->lblWinRateValue->setText(formatPercent(winRate));
    ui->lblCorrectValue->setText(QString::number(correct));
    ui->lblIncorrectValue->setText(QString::number(incorrect));
    ui->lblAccuracyValue->setText(formatPercent(accuracy));
    ui->lblAverageScoreValue->setText(QString::number(stats.average_score, 'f', 1));
    ui->lblRankedPointsValue->setText(QString::number(stats.ranked_points));
}
