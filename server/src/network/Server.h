#pragma once

#include <sys/epoll.h>
#include <map>
#include <memory>
#include "ClientSession.h"
#include "../game/RoomManager.h"

class Server {
public:
    Server(int port);
    ~Server();

    void run();
    RoomManager* getRoomManager();

private:
    void setNonBlocking(int fd);
    void handleAccept();

    int listenerFd;
    int epollFd;
    std::map<int, std::unique_ptr<ClientSession>> sessions;
    std::unique_ptr<RoomManager> roomManager;

    static const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];
};