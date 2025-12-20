#pragma once

#include <cstdint>

#define MAX_EMAIL_LEN 64
#define MAX_PASSWORD_LEN 64
#define MAX_DISPLAY_NAME_LEN 32
#define MAX_ROOM_NAME_LEN 32
#define MAX_QUESTION_CONTENT_LEN 256
#define MAX_OPTION_CONTENT_LEN 128
#define MAX_ROOMS_PER_LIST 50
#define MAX_PLAYERS_PER_ROOM 10
#define MAX_ERROR_MSG_LEN 128

enum class MessageType : uint16_t {
    C2S_REGISTER_REQ,
    S2C_REGISTER_RSP,
    C2S_LOGIN_REQ,
    S2C_LOGIN_RSP,
    C2S_ACCOUNT_UPDATE_REQ,
    S2C_ACCOUNT_UPDATE_RSP,

    C2S_LIST_ROOMS_REQ,
    S2C_LIST_ROOMS_RSP,
    C2S_CREATE_ROOM_REQ,
    S2C_CREATE_ROOM_RSP,
    C2S_JOIN_ROOM_REQ,
    S2C_JOIN_ROOM_RSP,
    S2C_PLAYER_JOINED_NOTIF,
    C2S_LEAVE_ROOM_REQ,
    S2C_PLAYER_LEFT_NOTIF,
    C2S_READY_STATUS_REQ,
    S2C_READY_STATUS_NOTIF,

    S2C_GAME_START_NOTIF,
    S2C_QUESTION_NOTIF,
    C2S_SUBMIT_ANSWER_REQ,
    S2C_ROUND_RESULT_NOTIF,
    S2C_PLAYER_ELIMINATED_NOTIF,
    S2C_GAME_OVER_NOTIF,
    C2S_LEAVE_MATCH_REQ,

    C2S_GET_STATS_REQ,
    S2C_GET_STATS_RSP,

    C2S_PAUSE_GAME_REQ,
    S2C_GAME_PAUSED_NOTIF,
    C2S_RESUME_GAME_REQ,
    S2C_GAME_RESUMED_NOTIF,
    S2C_GAME_TERMINATED_NOTIF,

    S2C_ERROR_RSP
};

enum class StatusCode : uint8_t {
    SUCCESS,
    FAILURE_GENERIC,
    EMAIL_EXISTS,
    INVALID_CREDENTIALS,
    ROOM_FULL,
    ROOM_NOT_FOUND,
    GAME_IN_PROGRESS,
    INVALID_REQUEST
};

enum class GameMode : uint8_t {
    ELIMINATION,
    SCORING
};


enum class TerminationReason : uint8_t {
    HOST_LEFT,
    NOT_ENOUGH_PLAYERS,
    SERVER_SHUTDOWN,
    UNKNOWN
};

#pragma pack(push, 1)

struct MessageHeader {
    MessageType type;
    uint32_t body_len;
};

struct RegisterRequest {
    char email[MAX_EMAIL_LEN];
    char display_name[MAX_DISPLAY_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
};

struct LoginRequest {
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
};

struct AccountUpdateRequest {
    char new_display_name[MAX_DISPLAY_NAME_LEN];
    // may add more options to change other stats
};

struct StatusResponse {
    StatusCode code;
};

struct LoginResponse {
    StatusCode code;
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
};

struct RoomInfo {
    uint32_t room_id;
    char room_name[MAX_ROOM_NAME_LEN];
    uint8_t current_players;
    uint8_t max_players;
    GameMode game_mode;
    bool is_in_game;
};

struct ListRoomsResponse {
    uint16_t room_count;
    RoomInfo rooms[MAX_ROOMS_PER_LIST];
};

struct CreateRoomRequest {
    char room_name[MAX_ROOM_NAME_LEN];
    GameMode game_mode;
    uint8_t num_questions;
};

struct CreateRoomResponse {
    StatusCode code;
    RoomInfo room_info;
};

struct JoinRoomRequest {
    uint32_t room_id;
};

struct PlayerInfo {
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
    bool is_ready;
};

struct JoinRoomResponse {
    StatusCode code;
    RoomInfo room_info;
    uint8_t player_count;
    PlayerInfo players[MAX_PLAYERS_PER_ROOM];
    uint32_t host_user_id;
};

struct PlayerLeftNotification {
    uint32_t user_id;
    uint32_t new_host_user_id;
};

struct ReadyStatusRequest {
    bool is_ready;
};

struct ReadyStatusNotification {
    uint32_t user_id;
    bool is_ready;
};

struct GameStartNotification {
    uint8_t player_count;
    PlayerInfo players[MAX_PLAYERS_PER_ROOM];
};

struct QuestionNotification {
    uint32_t question_id;
    uint8_t time_limit_sec;
    char content[MAX_QUESTION_CONTENT_LEN];
    char options[4][MAX_OPTION_CONTENT_LEN];
};

struct SubmitAnswerRequest {
    uint32_t question_id;
    uint8_t selected_option;
    uint32_t response_time_ms;
};

struct PlayerRoundResult {
    uint32_t user_id;
    int32_t score_change;
    uint32_t total_score;
    uint8_t correct_option;
    uint32_t points_for_this_question; 
    bool was_eliminated; 
    bool answered_question;
};

struct RoundResultNotification {
    uint8_t correct_option;
    uint8_t result_count;
    PlayerRoundResult results[MAX_PLAYERS_PER_ROOM];
};

struct PlayerEliminatedNotification {
    uint32_t user_id;
    uint32_t session_id;
};

struct PlayerFinalResult {
    uint32_t user_id;
    char display_name[MAX_DISPLAY_NAME_LEN];
    uint32_t final_rank;
    uint32_t final_score;
};

struct GameOverNotification {
    uint8_t result_count;
    PlayerFinalResult results[MAX_PLAYERS_PER_ROOM];
};

struct UserStatsResponse {
    uint32_t total_matches;
    uint32_t wins;
    uint32_t total_correct_answers;
    uint32_t total_incorrect_answers;
    double average_score;
    uint32_t ranked_points;
};

struct PauseGameRequest {
    // Empty body
};

struct GamePausedNotification {
    // Empty body
};

struct ResumeGameRequest {
    // Empty body
};

struct GameResumedNotification {
    // Empty body
};

struct GameTerminatedNotification {
    TerminationReason reason;
};

struct ErrorResponse {
    StatusCode code;
    char message[MAX_ERROR_MSG_LEN];
};

#pragma pack(pop)