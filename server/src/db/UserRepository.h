#pragma once

#include <bits/stdc++.h>
#include "protocol.h"

class UserRepository {
public:
    static UserStatsResponse getUserStats(uint32_t userId);

    struct RankUpdateInfo {
        uint32_t user_id;
        uint32_t rank;
        uint32_t score;
    };

    static void updateUserRanks(const std::vector<RankUpdateInfo>& matchResults);

private:
    static uint32_t calculateNewElo(uint32_t currentElo, uint32_t opponentElo, double actualScore);
};