#include "PosixSocketClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <poll.h>

PosixSocketClient::PosixSocketClient()
    : m_socketFd(-1) {
}

PosixSocketClient::~PosixSocketClient() {
    disconnect();
}

bool PosixSocketClient::connect(const std::string& host, uint16_t port) {
    if (m_socketFd != -1) {
        return true;
    }

    m_socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socketFd == -1) {
        perror("socket");
        return false;
    }

    struct addrinfo hints, *result, *p;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        perror("getaddrinfo");
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    for (p = result; p != nullptr; p = p->ai_next) {
        if (::connect(m_socketFd, p->ai_addr, p->ai_addrlen) == 0) {
            freeaddrinfo(result);
            int flags = fcntl(m_socketFd, F_GETFL, 0);
            fcntl(m_socketFd, F_SETFL, flags | O_NONBLOCK);
            return true;
        }
    }

    freeaddrinfo(result);
    close(m_socketFd);
    m_socketFd = -1;
    perror("connect");
    return false;
}

void PosixSocketClient::disconnect() {
    if (m_socketFd != -1) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

bool PosixSocketClient::isConnected() const {
    if (m_socketFd == -1) {
        return false;
    }

    struct pollfd pfd;
    pfd.fd = m_socketFd;
    pfd.events = POLLOUT;

    int ret = poll(&pfd, 1, 0);
    if (ret <= 0) {
        return false;
    }

    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(m_socketFd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
        return false;
    }

    return error == 0;
}

bool PosixSocketClient::send(const uint8_t* data, size_t len) {
    if (m_socketFd == -1) {
        return false;
    }

    size_t sent = 0;
    while (sent < len) {
        ssize_t n = ::send(m_socketFd, data + sent, len - sent, 0);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                struct pollfd pfd;
                pfd.fd = m_socketFd;
                pfd.events = POLLOUT;
                if (poll(&pfd, 1, 5000) <= 0) {
                    return false;
                }
                continue;
            }
            perror("send");
            return false;
        }
        sent += n;
    }

    return true;
}

int PosixSocketClient::recv(uint8_t* buffer, size_t maxLen) {
    if (m_socketFd == -1) {
        return -1;
    }

    ssize_t n = ::recv(m_socketFd, buffer, maxLen, 0);
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        perror("recv");
        return -1;
    }

    return n;
}
