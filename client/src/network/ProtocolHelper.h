#pragma once

#include <QByteArray>
#include <QString>
#include <cstring>
#include "protocol.h"

class ProtocolHelper {
public:
    // Pack any struct into QByteArray
    template<typename T>
    static QByteArray packStruct(const T& data) {
        return QByteArray(reinterpret_cast<const char*>(&data), sizeof(T));
    }
    
    // Unpack QByteArray into struct
    template<typename T>
    static T unpackStruct(const QByteArray& data) {
        T result;
        std::memset(&result, 0, sizeof(T));
        if (data.size() >= static_cast<int>(sizeof(T))) {
            std::memcpy(&result, data.constData(), sizeof(T));
        }
        return result;
    }
    
    // Create message with header
    static QByteArray createMessage(MessageType type, const QByteArray& body);
    
    // Convert QString to fixed-size char array (for protocol structs)
    static void copyToFixedArray(char* dest, int maxLen, const QString& src);
    
    // Convert fixed-size char array to QString
    static QString fromFixedArray(const char* src, int maxLen);
    
    // Byte order conversion (if needed for cross-platform)
    static uint32_t htonl(uint32_t hostlong);
    static uint32_t ntohl(uint32_t netlong);
    static uint16_t htons(uint16_t hostshort);
    static uint16_t ntohs(uint16_t netshort);
};
