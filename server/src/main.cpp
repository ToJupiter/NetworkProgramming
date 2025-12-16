#include "network/Server.h"
#include "db/DatabaseManager.h"
#include <sqlite_modern_cpp.h>
#include <iostream>

int main(int argc, char** argv) {
    DatabaseManager::getInstance();
    
    int port = 10002;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    Server server(port);
    server.run();

    return 0;
}