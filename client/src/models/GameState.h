#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <cstdint>
#include "protocol.h"

struct PlayerInfo {
    uint32_t userId;
    QString displayName;
    bool isReady;
    uint32_t score;
    bool isEliminated;
    
    PlayerInfo() : userId(0), isReady(false), score(0), isEliminated(false) {}
};

struct QuestionData {
    uint32_t questionId;
    QString content;
    QString options[4];
    uint8_t correctOption;  // Known after round ends
    uint32_t timeLimitSec;
    
    QuestionData() : questionId(0), correctOption(255), timeLimitSec(0) {}
};

class GameState : public QObject {
    Q_OBJECT
    
public:
    GameState(QObject* parent = nullptr);
    
    // Room info
    void setRoomInfo(uint32_t roomId, const QString& roomName, GameMode mode, uint8_t numQuestions);
    uint32_t getRoomId() const { return m_roomId; }
    QString getRoomName() const { return m_roomName; }
    GameMode getGameMode() const { return m_gameMode; }
    uint8_t getNumQuestions() const { return m_numQuestions; }
    
    // Players
    void clearPlayers();
    void addPlayer(const PlayerInfo& player);
    void removePlayer(uint32_t userId);
    void updatePlayerReady(uint32_t userId, bool ready);
    void updatePlayerScore(uint32_t userId, uint32_t score, bool eliminated);
    QVector<PlayerInfo> getPlayers() const { return m_players; }
    PlayerInfo* findPlayer(uint32_t userId);
    
    // Current question
    void setCurrentQuestion(const QuestionData& question);
    QuestionData getCurrentQuestion() const { return m_currentQuestion; }
    void setCurrentQuestionIndex(uint8_t index) { m_currentQuestionIndex = index; }
    uint8_t getCurrentQuestionIndex() const { return m_currentQuestionIndex; }
    
    // My answer
    void setMyAnswer(uint8_t option) { m_mySelectedOption = option; }
    uint8_t getMyAnswer() const { return m_mySelectedOption; }
    
    // My score
    void setMyScore(uint32_t score) { m_myScore = score; }
    uint32_t getMyScore() const { return m_myScore; }
    
    // Reset
    void clear();
    
signals:
    void playerListChanged();
    void questionChanged();
    void scoresUpdated();
    
private:
    uint32_t m_roomId;
    QString m_roomName;
    GameMode m_gameMode;
    uint8_t m_numQuestions;
    
    QVector<PlayerInfo> m_players;
    QuestionData m_currentQuestion;
    uint8_t m_currentQuestionIndex;
    
    uint8_t m_mySelectedOption;
    uint32_t m_myScore;
};
