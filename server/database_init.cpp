#include <iostream>
#include <sqlite_modern_cpp.h>
#include <fstream>
#include <sstream>

int main(int argc, char **argv) {
    try {
        std::string db_path = "build/game_db.db";
        sqlite::database db(db_path);

        // Read and execute SQL schema
        std::ifstream schema_file("../sql_schema/database_init.sql");
        if (!schema_file.is_open()) {
            std::cerr << "Error: Cannot open database_init.sql" << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << schema_file.rdbuf();
        std::string schema = buffer.str();

        // Split by semicolons and execute each statement
        std::istringstream iss(schema);
        std::string statement;
        std::string current_statement;

        while (std::getline(iss, statement, ';')) {
            current_statement += statement;
            if (current_statement.find("CREATE TABLE") != std::string::npos ||
                current_statement.find("INSERT INTO") != std::string::npos) {
                // Remove leading/trailing whitespace
                current_statement.erase(0, current_statement.find_first_not_of(" \t\n\r"));
                if (!current_statement.empty()) {
                    try {
                        db << current_statement;
                        std::cout << "Executed: " << current_statement.substr(0, 50) << "..." << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error executing statement: " << e.what() << std::endl;
                    }
                    current_statement.clear();
                }
            }
        }

        std::cout << "Database initialized successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "SQLite error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
