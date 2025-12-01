#include <iostream>
#include <sqlite_modern_cpp.h>
#include "bcrypt.h"

int main(int argc, char **argv){
    try {
        sqlite::database game_db("game_db.db");

        game_db << 
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "email TEXT UNIQUE NOT NULL,"
            "display_name TEXT NOT NULL,"
            "hashed_password TEXT NOT NULL,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";
        
        std::string name = "alice.dakinson@email.com";
        std::string display_name = "AliceDakinson";
        std::string hashed_password = "$2b$12$Xq.uAdsQ2EjdaBeMZuZDjumTw8eeELY0fm4seJfdhA91mX.mV.NeG";
        

        game_db << "INSERT INTO users (email, display_name, hashed_password) VALUES (?, ?, ?)"
                << name
                << display_name
                << hashed_password;
        

        game_db << "SELECT email, display_name, hashed_password, created_at FROM users"
                >> [&](std::string email, std::string db_display_name, std::string db_hashed_password, std::string created_at){
                    std::cout << email << ' ' << db_display_name << ' ' << db_hashed_password << ' ' << created_at << std::endl;
                };
    }
    catch (const std::exception& e) {
        std::cerr << "SQLite error: " << e.what() << std::endl;
        return 1;
    }


    // bcrypt testing
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    const char *password = "MyPassword123";

    bcrypt_gensalt(12, salt);
    printf("Generated salt: %s\n", salt);
    bcrypt_hashpw(password, salt, hash);
    printf("Generated hash: %s\n", hash);

    return 0;
}