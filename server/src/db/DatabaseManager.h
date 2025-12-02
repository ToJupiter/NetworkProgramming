#pragma once

#include <string>
#include <mutex>
#include <optional>
#include <memory>
#include <sqlite_modern_cpp.h>
#include "protocol.h"

struct UserData {
    uint32_t id;
    std::string email;
    std::string display_name;
};

class DatabaseManager {
public:
    static DatabaseManager& getInstance();
    
    StatusCode registerUser(const std::string& email, const std::string& display_name, const std::string& password);
    std::optional<UserData> loginUser(const std::string& email, const std::string& password);

private:
    DatabaseManager();
    ~DatabaseManager() = default;
    
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    std::unique_ptr<sqlite::database> db;
    std::mutex dbMutex;
};