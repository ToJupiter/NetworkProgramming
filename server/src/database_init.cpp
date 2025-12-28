#include "db/DatabaseManager.h"
#include <iostream>

void initializeDatabase() {
    try {
        auto& db = DatabaseManager::getInstance().getDb();
        
        db << "CREATE TABLE IF NOT EXISTS users ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "email TEXT UNIQUE NOT NULL, "
              "display_name TEXT NOT NULL, "
              "hashed_password TEXT NOT NULL, "
              "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "ranked_points INTEGER DEFAULT 1000"
              ");";

        // Questions Table
        db << "CREATE TABLE IF NOT EXISTS questions ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "content TEXT NOT NULL, "
              "option1 TEXT NOT NULL, "
              "option2 TEXT NOT NULL, "
              "option3 TEXT NOT NULL, "
              "option4 TEXT NOT NULL, "
              "correct_option INTEGER NOT NULL, "
              "difficulty INTEGER DEFAULT 1"
              ");";

        // Game Sessions Table
        // FIXED: Added total_pause_duration_ms to match Room.cpp persistence logic
        db << "CREATE TABLE IF NOT EXISTS game_sessions ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "game_mode TEXT NOT NULL, "
              "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "ended_at DATETIME"
              ");";

        // Session Participants Table
        db << "CREATE TABLE IF NOT EXISTS session_participants ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "user_id INTEGER NOT NULL, "
              "session_id INTEGER NOT NULL, "
              "score INTEGER DEFAULT 0, "
              "rank INTEGER, "
              "FOREIGN KEY(user_id) REFERENCES users(id), "
              "FOREIGN KEY(session_id) REFERENCES game_sessions(id)"
              ");";

        // Game Log Table
        db << "CREATE TABLE IF NOT EXISTS game_log ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "session_id INTEGER NOT NULL, "
              "user_id INTEGER NOT NULL, "
              "question_id INTEGER NOT NULL, "
              "selected_option INTEGER NOT NULL, "
              "is_correct BOOLEAN NOT NULL, "
              "response_time_ms INTEGER NOT NULL, "
              "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
              "FOREIGN KEY(session_id) REFERENCES game_sessions(id), "
              "FOREIGN KEY(user_id) REFERENCES users(id), "
              "FOREIGN KEY(question_id) REFERENCES questions(id)"
              ");";
              
        std::cout << "Database initialized successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Database Initialization Failed: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}