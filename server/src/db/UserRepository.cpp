#include "UserRepository.h"
#include "DatabaseManager.h"
#include <bits/stdc++.h>

UserStatsResponse UserRepository::getUserStats(uint32_t userId) {
    UserStatsResponse stats = {0, 0, 0, 0, 0.0, 1000};
    try {
        auto &db = DatabaseManager::getInstance().getDb();

        db << "SELECT ranked_points FROM users WHERE id = ?"
           << userId
           >> stats.ranked_points;
        
        db << "SELECT COUNT(*), SUM(CASE WHEN rank = 1 THEN 1 ELSE 0 END) "
              "FROM session_participants WHERE user_id = ?"
           << userId
           >> [&](int total, int wins) {
                stats.total_matches = total;
                stats.wins = wins;
           };

        db << "SELECT AVG(score) FROM session_participants WHERE user_id = ?"
           << userId
           >> [&](double avg) {
                stats.average_score = avg;
           };
        
        db << "SELECT SUM(CASE WHEN is_correct = 1 THEN 1 ELSE 0 END), "
              "       SUM(CASE WHEN is_correct = 0 THEN 1 ELSE 0 END)"
              "FROM game_log WHERE user_id = ?"
            << userId
            >> [&](int correct, int incorrect) {
                stats.total_correct_answers = correct;
                stats.total_incorrect_answers = incorrect;
            };
    } catch (const std::exception &e) {
        std::cerr << "DB Error getting stats: " << e.what() << std::endl;
    }
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