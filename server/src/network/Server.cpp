#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstring>

Server::Server(int port) {
    roomManager = std::make_unique<RoomManager>();

    listenerFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerFd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listenerFd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    setNonBlocking(listenerFd);

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listenerFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listenerFd, SOMAXCONN) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    epollFd = epoll_create1(0);
    if (epollFd < 0) {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenerFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, listenerFd, &ev);

    std::cout << "Server listening on port " << port << std::endl;
}

Server::~Server() {
    close(listenerFd);
    close(epollFd);
}

RoomManager* Server::getRoomManager(){
    return roomManager.get();
}

void Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::run() {
    while (true) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, 50);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == listenerFd) {
                handleAccept();
            } else {
                auto it = sessions.find(fd);
                if (it != sessions.end()) {
                    if (events[i].events & EPOLLIN) {
                        it->second->readData();
                    }
                    if (events[i].events & EPOLLOUT) {
                        it->second->writeData();
                    }
                    
                    if (it->second->isMarkedForDeletion()) {
                        it->second->onDisconnect();
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
                        sessions.erase(it);
                    } else {
                        struct epoll_event ev;
                        ev.events = EPOLLIN | EPOLLET;
                        if (it->second->wantWrite()) {
                            ev.events |= EPOLLOUT;
                        }
                        ev.data.fd = fd;
                        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
                    }
                }
            }
        }
        roomManager->updateAllRooms();
    }
}

void Server::handleAccept() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(listenerFd, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientFd < 0) return;

    setNonBlocking(clientFd);
    
    sessions[clientFd] = std::make_unique<ClientSession>(clientFd, this);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev);
}