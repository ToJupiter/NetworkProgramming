#include "SessionState.h"

SessionState::SessionState()
    : m_isAuthenticated(false)
    , m_userId(0)
    , m_currentRoomId(0)
{
}

void SessionState::clear() {
    m_isAuthenticated = false;
    m_userId = 0;
    m_displayName.clear();
    m_email.clear();
    m_currentRoomId = 0;
}
