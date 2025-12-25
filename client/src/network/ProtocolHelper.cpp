#include "ProtocolHelper.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

QByteArray ProtocolHelper::createMessage(MessageType type, const QByteArray& body) {
    MessageHeader header;
    header.type = type;
    header.body_len = htonl(static_cast<uint32_t>(body.size()));
    
    QByteArray message;
    message.append(reinterpret_cast<const char*>(&header), sizeof(MessageHeader));
    message.append(body);
    
    return message;
}

void ProtocolHelper::copyToFixedArray(char* dest, int maxLen, const QString& src) {
    QByteArray utf8 = src.toUtf8();
    int copyLen = qMin(maxLen - 1, utf8.size());
    std::memcpy(dest, utf8.constData(), copyLen);
    dest[copyLen] = '\0';
}

QString ProtocolHelper::fromFixedArray(const char* src, int maxLen) {
    // Find actual string length (up to null terminator or maxLen)
    int len = 0;
    while (len < maxLen && src[len] != '\0') {
        len++;
    }
    return QString::fromUtf8(src, len);
}

uint32_t ProtocolHelper::htonl(uint32_t hostlong) {
    return ::htonl(hostlong);
}

uint32_t ProtocolHelper::ntohl(uint32_t netlong) {
    return ::ntohl(netlong);
}

uint16_t ProtocolHelper::htons(uint16_t hostshort) {
    return ::htons(hostshort);
}

uint16_t ProtocolHelper::ntohs(uint16_t netshort) {
    return ::ntohs(netshort);
}
