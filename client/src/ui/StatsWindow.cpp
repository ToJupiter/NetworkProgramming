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
    const auto resetMode = [](QLabel* matches, QLabel* wins, QLabel* winRate,
                               QLabel* correct, QLabel* incorrect, QLabel* avgOrHigh = nullptr,
                               QLabel* highScore = nullptr) {
        matches->setText("Loading...");
        wins->setText("-");
        winRate->setText("-");
        correct->setText("-");
        incorrect->setText("-");
        if (avgOrHigh) avgOrHigh->setText("-");
        if (highScore) highScore->setText("-");
    };

    resetMode(ui->lblElimMatchesValue, ui->lblElimWinsValue, ui->lblElimWinRateValue,
              ui->lblElimCorrectValue, ui->lblElimIncorrectValue);

    resetMode(ui->lblScoreMatchesValue, ui->lblScoreWinsValue, ui->lblScoreWinRateValue,
              ui->lblScoreCorrectValue, ui->lblScoreIncorrectValue,
              ui->lblScoreAverageValue, ui->lblScoreHighValue);

    ui->lblRankedPointsValue->setText("-");
}

void StatsWindow::setStats(const UserStatsResponse &stats) {
    auto updateMode = [](const UserModeStats& mode, QLabel* matches, QLabel* wins,
                         QLabel* winRate, QLabel* correct, QLabel* incorrect,
                         QLabel* averageScore = nullptr, QLabel* highScore = nullptr) {
        const double rate = (mode.total_matches == 0)
                                ? 0.0
                                : (static_cast<double>(mode.wins) * 100.0 / mode.total_matches);

        matches->setText(QString::number(mode.total_matches));
        wins->setText(QString::number(mode.wins));
        winRate->setText(formatPercent(rate));
        correct->setText(QString::number(mode.total_correct_answers));
        incorrect->setText(QString::number(mode.total_incorrect_answers));
        if (averageScore) {
            averageScore->setText(QString::number(mode.average_score, 'f', 1));
        }
        if (highScore) {
            highScore->setText(QString::number(mode.high_score));
        }
    };

    updateMode(stats.elimination, ui->lblElimMatchesValue, ui->lblElimWinsValue,
               ui->lblElimWinRateValue, ui->lblElimCorrectValue, ui->lblElimIncorrectValue);

    updateMode(stats.scoring, ui->lblScoreMatchesValue, ui->lblScoreWinsValue,
               ui->lblScoreWinRateValue, ui->lblScoreCorrectValue, ui->lblScoreIncorrectValue,
               ui->lblScoreAverageValue, ui->lblScoreHighValue);

    ui->lblRankedPointsValue->setText(QString::number(stats.ranked_points));
}
