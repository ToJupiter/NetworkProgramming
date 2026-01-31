#include "DatabaseManager.h"
#include <iostream>
#include <functional>
#include "bcrypt.h"

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    try {
        db = std::make_unique<sqlite::database>("game_db.db");
        *db << "PRAGMA journal_mode = WAL;";
        *db << "PRAGMA synchronous = NORMAL;";
    } catch (const std::exception& e) {
        std::cerr << "Database Init Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

StatusCode DatabaseManager::registerUser(const std::string& email, const std::string& displayName, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    try {
        int count = 0;
        *db << "SELECT COUNT(*) FROM users WHERE email = ?" << email >> count;
        if (count > 0) return StatusCode::EMAIL_EXISTS;

        char salt[BCRYPT_HASHSIZE];
        char hash[BCRYPT_HASHSIZE];
        bcrypt_gensalt(12, salt);
        bcrypt_hashpw(password.c_str(), salt, hash);

        *db << "INSERT INTO users (email, display_name, hashed_password) VALUES (?, ?, ?)"
            << email << displayName << hash;

        return StatusCode::SUCCESS;
    } catch (const std::exception&) {
        return StatusCode::FAILURE_GENERIC;
    }
}

std::optional<UserData> DatabaseManager::loginUser(const std::string& email, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    try {
        std::string dbHash;
        uint32_t id = 0;
        std::string dbInfoName;
        bool found = false;

        *db << "SELECT id, display_name, hashed_password FROM users WHERE email = ?" 
            << email 
            >> [&](uint32_t u_id, std::string d_name, std::string h_pass) {
                id = u_id;
                dbInfoName = d_name;
                dbHash = h_pass;
                found = true;
            };

        if (found) {
            int password_match = bcrypt_checkpw(password.c_str(), dbHash.c_str());
            if (password_match == 0){
                return UserData{id, email, dbInfoName};
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Login Error: " << e.what() << std::endl;
    }
    return std::nullopt;
}