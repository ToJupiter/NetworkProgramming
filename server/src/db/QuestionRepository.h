#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "../../common/protocol.h"

struct Question {
    uint32_t id;
    std::string content;
    std::string options[4];
    uint8_t correct_option;
    uint8_t difficulty;
};

class QuestionRepository {
public:
    std::vector<Question> getRandomQuestions(int count);
};