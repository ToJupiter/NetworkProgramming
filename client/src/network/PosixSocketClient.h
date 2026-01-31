#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

class PosixSocketClient {
public:
    PosixSocketClient();
    ~PosixSocketClient();

    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const;

    bool send(const uint8_t* data, size_t len);
    int recv(uint8_t* buffer, size_t maxLen);

    int getFd() const { return m_socketFd; }

private:
    int m_socketFd;
};
