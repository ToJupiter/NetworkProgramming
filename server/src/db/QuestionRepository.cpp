#include "QuestionRepository.h"
#include "DatabaseManager.h"
#include <iostream>

std::vector<Question> QuestionRepository::getRandomQuestions(int count) {
    std::vector<Question> questions;
    try {
        auto& db = DatabaseManager::getInstance().getDb();

        db << "SELECT id, content, option1, option2, option3, option4, correct_option, difficulty "
              "FROM questions ORDER BY RANDOM() LIMIT ?"
           << count
           >> [&](uint32_t id, std::string content, std::string o1, std::string o2, std::string o3, std::string o4, int correct, int diff) {
               Question q;
               q.id = id;
               q.content = content;
               q.options[0] = o1;
               q.options[1] = o2;
               q.options[2] = o3;
               q.options[3] = o4;
               q.correct_option = static_cast<uint8_t>(correct);
               q.difficulty = static_cast<uint8_t>(diff);
               questions.push_back(q);
           };

    } catch (const std::exception &e) {
        std::cerr << "DB Error fetching questions: " << e.what() << std::endl;
    }

    return questions;
}