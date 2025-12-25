#include "GameState.h"

GameState::GameState(QObject* parent)
    : QObject(parent)
    , m_roomId(0)
    , m_gameMode(GameMode::ELIMINATION)
    , m_numQuestions(0)
    , m_currentQuestionIndex(0)
    , m_mySelectedOption(0)
    , m_myScore(0)
{
}

void GameState::setRoomInfo(uint32_t roomId, const QString& roomName, GameMode mode, uint8_t numQuestions) {
    m_roomId = roomId;
    m_roomName = roomName;
    m_gameMode = mode;
    m_numQuestions = numQuestions;
}

void GameState::clearPlayers() {
    m_players.clear();
    emit playerListChanged();
}

void GameState::addPlayer(const PlayerInfo& player) {
    m_players.append(player);
    emit playerListChanged();
}

void GameState::removePlayer(uint32_t userId) {
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i].userId == userId) {
            m_players.removeAt(i);
            emit playerListChanged();
            break;
        }
    }
}

void GameState::updatePlayerReady(uint32_t userId, bool ready) {
    PlayerInfo* player = findPlayer(userId);
    if (player) {
        player->isReady = ready;
        emit playerListChanged();
    }
}

void GameState::updatePlayerScore(uint32_t userId, uint32_t score, bool eliminated) {
    PlayerInfo* player = findPlayer(userId);
    if (player) {
        player->score = score;
        player->isEliminated = eliminated;
        emit scoresUpdated();
    }
}

PlayerInfo* GameState::findPlayer(uint32_t userId) {
    for (int i = 0; i < m_players.size(); ++i) {
        if (m_players[i].userId == userId) {
            return &m_players[i];
        }
    }
    return nullptr;
}

void GameState::setCurrentQuestion(const QuestionData& question) {
    m_currentQuestion = question;
    m_mySelectedOption = 0;
    emit questionChanged();
}

void GameState::clear() {
    m_roomId = 0;
    m_roomName.clear();
    m_numQuestions = 0;
    m_currentQuestionIndex = 0;
    m_mySelectedOption = 0;
    m_myScore = 0;
    clearPlayers();
}
