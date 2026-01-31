#pragma once

#include <QString>
#include <cstdint>

class SessionState {
public:
    SessionState();
    // Global singleton access
    static SessionState& instance();
    
    // Authentication state
    bool isAuthenticated() const { return m_isAuthenticated; }
    void setAuthenticated(bool auth) { m_isAuthenticated = auth; }
    
    // User info
    uint32_t getUserId() const { return m_userId; }
    void setUserId(uint32_t id) { m_userId = id; }
    
    QString getDisplayName() const { return m_displayName; }
    void setDisplayName(const QString& name) { m_displayName = name; }
    
    QString getEmail() const { return m_email; }
    void setEmail(const QString& email) { m_email = email; }
    
    // Room state
    uint32_t getCurrentRoomId() const { return m_currentRoomId; }
    void setCurrentRoomId(uint32_t id) { m_currentRoomId = id; }
    
    bool isInRoom() const { return m_currentRoomId != 0; }
    
    // Reset
    void clear();
    
private:
    bool m_isAuthenticated;
    uint32_t m_userId;
    QString m_displayName;
    QString m_email;
    uint32_t m_currentRoomId;
};
