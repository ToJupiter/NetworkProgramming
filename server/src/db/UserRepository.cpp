#include "UserRepository.h"
#include "DatabaseManager.h"
#include <bits/stdc++.h>
#include <algorithm>

UserStatsResponse UserRepository::getUserStats(uint32_t userId) {
    UserStatsResponse stats{};
    // Default ranked points if no record found
    stats.ranked_points = 1000;

    auto fillModeStats = [&](const std::string& mode, UserModeStats& out) {
        out.total_matches = 0;
        out.wins = 0;
        out.total_correct_answers = 0;
        out.total_incorrect_answers = 0;
        out.average_score = 0.0;
        out.high_score = 0;

        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT COUNT(*), IFNULL(SUM(CASE WHEN rank = 1 THEN 1 ELSE 0 END), 0) "
              "FROM session_participants sp "
              "JOIN game_sessions gs ON sp.session_id = gs.id "
              "WHERE sp.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](int total, int wins) {
                out.total_matches = total;
                out.wins = wins;
           };

        db << "SELECT IFNULL(AVG(sp.score), 0), IFNULL(MAX(sp.score), 0) "
              "FROM session_participants sp "
              "JOIN game_sessions gs ON sp.session_id = gs.id "
              "WHERE sp.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](double avg, int maxScore) {
                out.average_score = avg;
                out.high_score = static_cast<uint32_t>(std::max(0, maxScore));
           };

        db << "SELECT IFNULL(SUM(CASE WHEN gl.is_correct = 1 THEN 1 ELSE 0 END), 0), "
              "       IFNULL(SUM(CASE WHEN gl.is_correct = 0 THEN 1 ELSE 0 END), 0) "
              "FROM game_log gl "
              "JOIN game_sessions gs ON gl.session_id = gs.id "
              "WHERE gl.user_id = ? AND gs.game_mode = ?"
           << userId << mode
           >> [&](int correct, int incorrect) {
                out.total_correct_answers = correct;
                out.total_incorrect_answers = incorrect;
           };
    };

    try {
        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT ranked_points FROM users WHERE id = ?"
           << userId
           >> stats.ranked_points;

        fillModeStats("Elimination", stats.elimination);
        fillModeStats("Scoring", stats.scoring);
    } catch (const std::exception &e) {
        std::cerr << "DB Error getting stats: " << e.what() << std::endl;
    }

    return stats;
}

void UserRepository::updateUserRanks(const std::vector<RankUpdateInfo>& results) {
    if (results.size() < 2) return;

    try {
        auto &db = DatabaseManager::getInstance().getDb();
        db << "BEGIN TRANSACTION;";

        uint64_t totalElo = 0;
        std::vector<uint32_t> currentElos;

        for (const auto &p : results) {
            uint32_t elo = 1000;
            db << "SELECT ranked_points FROM users WHERE id = ?" << p.user_id >> elo;
            currentElos.push_back(elo);
            totalElo += elo;
        }

        double avgElo = static_cast<double>(totalElo) / results.size();

        for (size_t i = 0; i < results.size(); i++) {
            uint32_t userId = results[i].user_id;
            uint32_t oldElo = currentElos[i];
            uint32_t rank = results[i].rank;

            double actualScore = 1.0 - (double)(rank - 1) / (results.size() - 1);
            if (results.size() == 1) actualScore = 1.0;
            
            double expectedScore = 1.0 / (1.0 + pow(10.0, (avgElo - oldElo) / 400.0));
            int k = 32;

            int change = static_cast<int>(k * (actualScore - expectedScore));
            int newElo = static_cast<int>(oldElo) + change;
            if (newElo < 0) newElo = 0;


            db << "UPDATE users SET ranked_points = ? WHERE id = ?" 
               << newElo << userId;
        }
        
        db << "COMMIT;";
    } catch (const std::exception &e) {
        std::cerr << "DB Error updating ranks: " << e.what() << std::endl;
        DatabaseManager::getInstance().getDb() << "ROLLBACK;";
    }
}