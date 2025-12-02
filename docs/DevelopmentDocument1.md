# **Multiplayer Quiz Platform - Development Technical Documentation**
## **Engineering Specification v1.0 - Core Systems & Network Layer**

---

## **1. STREAM HANDLING & SOCKET I/O IMPLEMENTATION**

### **1.1. TCP Transport Configuration & Optimization**

**Engineering Decision:** TCP over UDP due to requirement of 100% answer delivery reliability and ordered message processing. UDP would require custom retransmission logic, increasing complexity by 3x.

**Socket Initialization (Server-Side):**
```cpp
// server_core/network/SocketManager.cpp
int SocketManager::initializeListener(uint16_t port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) throw NetworkException("Socket creation failed");
    
    // Critical performance flags
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)); // Disable Nagle
    setsockopt(listen_fd, SOL_SOCKET, SO_RCVBUF, &opt, 128 * 1024);   // 128KB buffer
    setsockopt(listen_fd, SOL_SOCKET, SO_SNDBUF, &opt, 128 * 1024);   // 128KB buffer
    
    // Non-blocking mode for scalability
    int flags = fcntl(listen_fd, F_GETFL, 0);
    fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK);
    
    bind(listen_fd, ...);
    listen(listen_fd, SOMAXCONN); // Kernel backlog queue
    
    return listen_fd;
}
```

**Connection State Machine (Per Client):**
```
CONNECTED → AUTHENTICATED → IN_LOBBY → [IN_ROOM | IN_GAME] → DISCONNECTED
      │          │             │            │
      └──5s T/O──┴────30s T/O──┴────────────┘
```

**State Transition Details:**
- **HANDSHAKING → ESTABLISHED:** 3-way TCP handshake completion, socket moved to epoll queue
- **ESTABLISHED → AUTHENTICATED:** First message must be `C2S_REGISTER_REQ` or `C2S_LOGIN_REQ` within 30s
- **IN_LOBBY → IN_ROOM:** User joins room, socket registered to room-specific broadcast group

### **1.2. Message Framing Protocol**

**Frame Structure:**
```
[6-byte Header] + [Variable-length Body]

Header:
┌───────────────┬───────────────┐
│  MessageType  │   Body Length │
│  (uint16_t)   │  (uint32_t)   │
└───────────────┴───────────────┘
  2 bytes         4 bytes (network byte order)

Body:
┌───────────────┐
│   Packed      │
│   Struct      │
│  (pragma pack │
│    (1))       │
└───────────────┘
```

**Engineering Considerations:**
- **Max Message Size:** 4,102 bytes (largest: `S2C_LIST_ROOMS_RSP` = 6 + 50×56 + 2 = 2,808 bytes)
- **Fragmentation Handling:** Each connection maintains `recv_buffer[8192]` and `bytes_received` counter. Implement `while(bytes_received < header.body_len)` loop with **50ms timeout** per iteration.
- **Byte Order:** All multi-byte values use **network byte order (big-endian)**. Conversion functions:
```cpp
uint32_t hton32(uint32_t host) { return htonl(host); }
uint32_t ntoh32(uint32_t net) { return ntohl(net); }
```

### **1.3. Asynchronous I/O with epoll (Linux)**

**Model:**
```
┌─────────────────────────────────────────────────┐
│               Game Server (Main Thread)         │
│  ┌───────────────┐  ┌────────────────────────┐ │
│  │  epoll/kqueue │  │    Game Room Manager   │ │
│  │  Event Loop   │  │   - Creates/destroys   │ │
│  │               │  │   - Routes messages     │ │
│  └───────────────┘  └────────────────────────┘ │
│           │                     │               │
│  ┌────────────────────────────────────────────┐│
│  │         Worker Thread Pool (4-8 threads)   ││
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌──────┐││
│  │  │Worker1 │ │Worker2 │ │Worker3 │ │ ...  │││
│  │  └────────┘ └────────┘ └────────┘ └──────┘││
│  └────────────────────────────────────────────┘│
└─────────────────────────────────────────────────┘
```

**Scalability Model:**
- **Target:** 500 concurrent clients per server instance
- **Thread Pool:** 4 worker threads (1 per CPU core) processing epoll events
- **Epoll Configuration:**
```cpp
// server_core/network/EPollManager.cpp
int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
struct epoll_event ev;
ev.events = EPOLLIN | EPOLLET; // Edge-triggered mode (performance +30%)
ev.data.fd = client_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
```

**Edge-Triggered (ET) Mode Considerations:**
- Must read/write **until EAGAIN** returned
- Requires non-blocking sockets
- Reduces epoll_wait() calls by **factor of 10** compared to level-triggered
- Should be using if/else blocks to handle this.

---

## **2. ACCOUNT REGISTRATION & MANAGEMENT**

### **2.1. Registration Flow State Machine**

```
┌──────────────┐
│  REG_INIT    │──┐
└──────┬───────┘  │
       │C2S_REGISTER_REQ
       ▼
┌──────────────┐   Validation Fail
│  VALIDATING  │──┐──────────────┐
└──────┬───────┘  │              │
       │Success   │              │
       ▼          │              │
┌──────────────┐  │              │
│  DB_INSERT   │  │              │
└──────┬───────┘  │              │
       │           │              │
       │           │              │
       ▼          ▼              │
┌────────────────────┐           │
│  REG_COMPLETE      │◄──────────┘
└────────┬───────────┘
         │
         └─▶ [Auto-login to LOGGED_IN state]
```

### **2.2. Input Validation Matrix**

| Field | Length | Format | SQL Injection | Performance |
|-------|--------|--------|---------------|-------------|
| email | 3-64 | RFC 5322 subset | Parameterized | O(log n) index |
| password | 8-64 | Entropy ≥ 32 bits | N/A (bcrypt) | 250ms hash time |
| display_name | 3-32 | Unicode allowed | Escape HTML | O(1) length check |

**Email Validation Regex:** 
```cpp
const std::regex email_pattern("^[a-zA-Z0-9._%+-]{1,64}@[a-zA-Z0-9.-]{1,63}\\.[a-zA-Z]{2,}$");
```

### **2.3. Database Transaction for Registration**

```cpp
// server_core/auth/AuthManager.cpp
StatusCode AuthManager::registerUser(const RegisterRequest& req) {
    sqlite::database& db = DatabasePool::get();
    
    try {
        db << "BEGIN IMMEDIATE TRANSACTION";  // Prevents deadlocks
        
        // Check for existing email (case-insensitive)
        int exists = 0;
        db << "SELECT COUNT(*) FROM users WHERE lower(email) = lower(?)" 
           << req.email >> exists;
        
        if (exists > 0) {
            db << "ROLLBACK";
            return StatusCode::EMAIL_EXISTS;
        }
        
        // Hash password (bcrypt)
        char salt[BCRYPT_HASHSIZE];
        char hash[BCRYPT_HASHSIZE];
        bcrypt_gensalt(12, salt);  // 2^12 rounds
        bcrypt_hashpw(req.password, salt, hash);
        
        // Insert user
        db << "INSERT INTO users (email, display_name, hashed_password) VALUES (?, ?, ?)"
           << req.email << req.display_name << std::string(hash);
        
        db << "COMMIT";
        return StatusCode::SUCCESS;
        
    } catch (sqlite::sqlite_exception& e) {
        db << "ROLLBACK";
        Logger::error("Registration DB error: {}", e.what());
        return StatusCode::FAILURE_GENERIC;
    }
}
```

**Performance Target:** Registration must complete in **<500ms** (including bcrypt hash).

---

## **3. LOGIN & SESSION MANAGEMENT**

### **3.1. Login Flow State Machine**

```
┌──────────────┐
│  LOGIN_INIT  │
└──────┬───────┘
       │
       │ C2S_LOGIN_REQ
       ▼
┌──────────────┐
│  CREDENTIAL  │──┐
│  VERIFICATION│  │ Hash mismatch
└──────┬───────┘  │
       │ Success  │
       ▼          │
┌──────────────┐  │
│  SESSION     │  │
│  CREATION    │  │
└──────┬───────┘  │
       │           │
       └───────────┴──┐
                      │
       ▼              ▼
┌─────────────────────────┐
│  LOGGED_IN              │
│  (Session Token Active) │
└────────┬────────────────┘
         │
         │ Inactivity > 30min
         ▼
┌─────────────────────────┐
│  SESSION_EXPIRED        │
└─────────────────────────┘
```

### **3.2. Session Token Implementation**

**Token Structure (In-Memory):**
```cpp
struct SessionToken {
    uint32_t user_id;
    char display_name[32];
    uint64_t created_at;      // epoch ms
    uint64_t last_activity;
    uint32_t current_room_id; // 0 = not in room
    uint32_t ip_address;
    
    bool isExpired() const {
        return (current_time_ms() - last_activity) > 1800000; // 30min
    }
};
```

**Storage:**
- **Active Sessions:** `std::unordered_map<uint32_t, SessionToken> active_sessions;` (key: user_id)
- **Locking:** `std::shared_mutex session_mutex;` for thread-safe access
- **Cleanup:** Dedicated thread calls `purgeExpiredSessions()` every **5 minutes**

**Token Generation Algorithm:**
```cpp
uint32_t generateSessionToken(uint32_t user_id) {
    // Simple deterministic token: user_id + timestamp_hash
    uint64_t timestamp = current_time_ms();
    uint32_t token = user_id ^ (timestamp & 0xFFFFFFFF);
    return token;
}
```
**Security Note:** This is internal session tracking; for distributed systems, use JWT with HMAC256.

---

## **4. ROOM MANAGEMENT SYSTEM**

### **4.1. Room Lifecycle State Machine**

```
ROOM_CREATED (Entry)
  └─► Actions: Allocate Room struct, assign room_id (atomic counter)
      Transitions: on(C2S_JOIN_ROOM_REQ) → ROOM_WAITING
                   on(C2S_READY_STATUS_REQ) → [Error: not enough players]

ROOM_WAITING (Duration: 0-300s)
  └─► Properties: 
        - current_players: 1-10
        - ready_mask: 10-bit bitmask (player indices)
        - broadcast_interval: 500ms (S2C_PLAYER_JOINED_NOTIF)
      Transitions: 
        - on(C2S_JOIN_ROOM_REQ) → if(current_players < 10) update roster
        - on(C2S_LEAVE_ROOM_REQ) → current_players-- → if(==0) → ROOM_DESTROYED
        - on(C2S_READY_STATUS_REQ) → if(popcount(ready_mask) == current_players >= 2) → ROOM_STARTING
      Timeout: 300s with <2 players → ROOM_DESTROYED

ROOM_STARTING (Fixed Duration: 5s)
  └─► Actions:
        T=0.0s: S2C_GAME_START_NOTIF
        T=5.0s: Generate question queue (SELECT ... ORDER BY RANDOM() LIMIT n)
      Transitions: T=5.0s → ROOM_IN_GAME

ROOM_IN_GAME (Sub-state machine)
  ├─► QUESTION_DISPLAY (3-5s)
  ├─► ANSWER_ACCEPTING (5-30s, mode-dependent)
  ├─► ROUND_PROCESSING (1-2s calculation)
  └─► Transitions: 
        - on(C2S_SUBMIT_ANSWER_REQ) → validate timestamp → store
        - if(active_players == 1 && mode == ELIMINATION) → ROOM_COMPLETED
        - if(question_index == num_questions) → ROOM_COMPLETED

ROOM_COMPLETED (Duration: 10-30s)
  └─► Actions: Calculate ranks, update statistics, persist game_log
      Transitions: After 30s → ROOM_DESTROYED

ROOM_DESTROYED (Exit)
  └─► Actions: Deallocate memory, broadcast termination, notify players
```

### **4.2. Room Creation Logic**

```cpp
// server_core/room/RoomManager.cpp
CreateRoomResponse RoomManager::createRoom(const CreateRoomRequest& req, uint32_t host_id) {
    // Validate parameters
    if (strlen(req.room_name) < 3) {
        return {StatusCode::INVALID_REQUEST, {}};
    }
    
    // Allocate room (thread-safe)
    Room* room = new Room();
    room->room_id = atomic_fetch_add(&next_room_id, 1);
    room->host_user_id = host_id;
    room->game_mode = req.game_mode;
    room->num_questions = req.num_questions;
    room->max_players = MAX_PLAYERS_PER_ROOM;
    room->current_players = 1;
    room->state = ROOM_WAITING;
    
    // Add host to participants
    room->participants[0] = {host_id, true}; // host auto-ready
    
    // Insert to active_rooms map
    {
        std::lock_guard<std::mutex> lock(rooms_mutex);
        active_rooms[room->room_id] = room;
    }
    
    // Broadcast to lobby
    broadcastRoomListUpdate();
    
    return {StatusCode::SUCCESS, room->toRoomInfo()};
}
```

**Thread Safety:** `rooms_mutex` protects `active_rooms` unordered_map. **Read-write lock** alternative for better concurrency: `std::shared_mutex`.

---

## **5. REAL-TIME GAMEPLAY MANAGEMENT** (8-POINT COMPLEXITY)

### **5.1. Game Session Sub-State Machine (Per Question)**

```
QUESTION_QUEUED
  └─► Server selects question from pool
      Transition: on(TIMER_EXPIRE) → QUESTION_DISPATCHING

QUESTION_DISPATCHING (Duration: 0ms - network latency)
  └─► Actions:
        - Record dispatch timestamp per player (map: user_id → sent_time_ms)
        - Send S2C_QUESTION_NOTIF to all participants
        - Start answer window timer
      Transition: on(dispatch_complete) → ANSWER_WINDOW_OPEN

ANSWER_WINDOW_OPEN (Duration: time_limit_sec)
  └─► Properties:
        - Accept C2S_SUBMIT_ANSWER_REQ
        - Validate: timestamp ∈ [0, time_limit_ms + 500] (500ms grace for latency)
        - Store answers in pending_answers vector
      Transition: on(TIMER_EXPIRE) → ANSWER_WINDOW_CLOSED
                   or on(all_players_answered) → ANSWER_WINDOW_CLOSED

ANSWER_WINDOW_CLOSED
  └─► Actions:
        - Lock pending_answers (no more submissions)
        - Validate timestamps against server-sent time
        - Calculate scores per mode
        - Determine eliminations (if ELIMINATION mode)
      Transition: on(calculation_complete) → RESULT_DISPATCHING

RESULT_DISPATCHING
  └─► Actions:
        - Build S2C_ROUND_RESULT_NOTIF packet
        - Broadcast to all participants
        - Update session_participants.score in DB (batch update)
      Transition: on(dispatch_complete) → QUESTION_QUEUED (next question)
                   or on(last_question) → GAME_COMPLETION
```

### **5.2. Timestamp Validation & Anti-Lag Compensation**

```cpp
// server_core/gameplay/GameSession.cpp
void GameSession::validateAnswerTimestamp(uint32_t user_id, 
                                          SubmitAnswerRequest& req) {
    auto server_sent_time = question_dispatch_times[user_id];
    auto server_receive_time = current_time_ms();
    
    // Client-reported time
    uint32_t client_response = req.response_time_ms;
    
    // Server-measured RTT
    uint32_t measured_latency = server_receive_time - server_sent_time;
    
    // Acceptable bounds: [0, time_limit + 500ms]
    uint32_t max_allowed = question_time_limit + 500;
    
    if (client_response > max_allowed) {
        // Client clock ahead or manipulation
        logCheatAttempt(user_id, "Excessive response time");
        req.response_time_ms = measured_latency; // Override
    }
    
    if (client_response < measured_latency - 100) {
        // Client clock behind or packet replay
        logCheatAttempt(user_id, "Impossible timestamp");
        req.response_time_ms = measured_latency;
    }
    
    // RTT adjustment for fair scoring
    uint32_t adjusted_time = client_response - (getRTT(user_id) / 2);
    req.response_time_ms = clamp(adjusted_time, 0u, question_time_limit);
}
```

**RTT Calculation:** 
- Sampled every 10 seconds via ping-pong message
- Stored as moving average: `rtt = (rtt * 0.8) + (new_sample * 0.2)`

### **5.3. Concurrent Answer Processing**

**Problem:** 10 players submit answers simultaneously → 10 threads hit DB.

**Solution: Batch Insert Pattern**
```cpp
// Collect answers in memory
std::vector<GameLogEntry> pending_logs;

// After answer window closes:
void GameSession::flushGameLogs() {
    sqlite::database& db = DatabasePool::get();
    db << "BEGIN TRANSACTION";
    
    for (const auto& log : pending_logs) {
        db << "INSERT INTO game_log VALUES (?, ?, ?, ?, ?, ?)"
           << log.session_id << log.user_id << log.question_id
           << log.selected_option << log.is_correct << log.response_time_ms;
    }
    
    db << "COMMIT"; // Single commit for all 10 inserts: 5ms vs 50ms individual
}
```

**Performance Gain:** **10x faster** (batched write vs individual transactions)

---

## **6. GAME MODE IMPLEMENTATIONS**

### **6.1. Elimination Mode - Deterministic Logic**

**Elimination Rule:**
```cpp
struct EliminationResult {
    bool is_eliminated;
    uint8_t new_rank; // For leaderboard update
};

EliminationResult processElimination(const PlayerRoundResult& result,
                                     uint8_t current_active_count) {
    if (!result.answered_question) {
        return {true, current_active_count}; // Timeout = elimination
    }
    if (!result.is_correct) {
        return {true, current_active_count}; // Wrong answer = elimination
    }
    return {false, 0}; // Survives
}
```

**Rank Assignment:** 
- Last survivor: rank = 1
- Eliminated players: rank = question_index when eliminated (2-10)
- Tie-breaker: Faster response time = better rank

### **6.2. Scoring Mode - Multi-Variable Formula**

```cpp
// server_core/gameplay/ScoringEngine.h
#include <cmath>
#include <vector>

class KahootScorer {
private:
    int maxPoints;          // P_max
    int minPoints;          // P_min
    int maxTime;            // t_max
    double decayConstant;   // k
    double streakBonus;     // bonus per streak
    double maxMultiplier;   // maximum streak multiplier
    
    std::vector<int> playerStreaks;  // streak count for each player
    
public:
    KahootScorer(int numPlayers, int maxPts = 1000, int minPts = 500, 
                 int timeLimit = 30, double k = 2.0, 
                 double streakBonus = 0.1, double maxMult = 1.5)
        : maxPoints(maxPts), minPoints(minPts), maxTime(timeLimit),
          decayConstant(k), streakBonus(streakBonus), maxMultiplier(maxMult),
          playerStreaks(numPlayers, 0) {}
    
    // Calculate points for a single question
    int calculateScore(int playerId, bool correct, int answerTime) {
        if (!correct || answerTime > maxTime) {
            playerStreaks[playerId] = 0;
            return 0;
        }
        
        // Time-based decay (power law)
        double timeRatio = static_cast<double>(answerTime) / maxTime;
        double decay = 1.0 - pow(timeRatio, decayConstant);
        double basePoints = minPoints + decay * (maxPoints - minPoints);
        
        // Streak multiplier
        int streak = ++playerStreaks[playerId];
        double multiplier = 1.0 + (streak - 1) * streakBonus;
        multiplier = std::min(multiplier, maxMultiplier);
        
        return static_cast<int>(std::round(basePoints * multiplier));
    }
    
    // Reset player streaks (e.g., for new game)
    void resetStreaks() {
        std::fill(playerStreaks.begin(), playerStreaks.end(), 0);
    }
    
    // Set points multiplier (Standard/Double/No points)
    void setPointsMode(bool doublePoints, bool noPoints) {
        if (noPoints) {
            maxPoints = 0;
            minPoints = 0;
        } else if (doublePoints) {
            maxPoints = 2000;
            minPoints = 1000;
        } else {
            maxPoints = 1000;
            minPoints = 500;
        }
    }
};
```

**Integer Precision:** Final score cast to `uint32_t` with rounding: `(uint32_t)(score + 0.5)`

---

## **7. PAUSE/STOP GAME FUNCTIONALITY**

### **7.1. Pause Mechanism (Host-Only)**

**State Machine Integration:**
```
ROOM_IN_GAME → PAUSED (on C2S_PAUSE_GAME_REQ from host)
  └─► Actions:
        - Halt all timers (question, answer window)
        - Send S2C_GAME_PAUSED_NOTIF to all
        - Record pause duration for fairness
  
PAUSED → RESUMED (on C2S_RESUME_GAME_REQ from host)
  └─► Actions:
        - Resume timers with adjusted remaining time
        - Send S2C_GAME_RESUMED_NOTIF
```

**Database Logging:**
```cpp
// In game_sessions table
ALTER TABLE game_sessions ADD COLUMN total_pause_duration_ms INTEGER DEFAULT 0;
```

**Anti-Abuse:** Max pause time = **60 seconds per game**. Exceeding auto-resumes.

### **7.2. Early Termination (Host or System)**

```cpp
void GameSession::terminateGame(TerminationReason reason) {
    // Determine final ranks based on current state
    if (reason == HOST_LEFT) {
        // Host forfeits, remaining players tie for 1st
        assignTiedRanks();
    }
    
    // Persist incomplete session with special flag
    db << "UPDATE game_sessions SET ended_at = ?, termination_reason = ? "
          "WHERE id = ?" << now() << reason << session_id;
    
    // Send S2C_GAME_TERMINATED_NOTIF
    broadcast(MessageType::S2C_GAME_TERMINATED_NOTIF, {reason});
    
    state = ROOM_DESTROYED;
}
```

---

## **8. SAVE SCORES & LEADERBOARDS**

### **8.1. Score Persistence Strategy**

**Write-Ahead Caching:**
- **Memory Cache:** `std::unordered_map<uint32_t, uint32_t> live_scores;` (user_id → score)
- **Flush Interval:** Every **question completion** (not per answer) to reduce DB writes by **80%**
- **Atomic Update:**
```cpp
db << "UPDATE session_participants SET score = ? "
      "WHERE session_id = ? AND user_id = ?"
   << new_score << session_id << user_id;
```

### **8.2. Leaderboard Real-Time Update**

**Broadcast Frequency:** Every **500ms** during gameplay, immediately after round ends.

**Packet Structure:**
```cpp
struct LeaderboardEntry {
    uint32_t user_id;
    uint32_t current_score;
    uint8_t rank;
} __attribute__((packed));

struct LeaderboardUpdate {
    uint8_t entry_count;
    LeaderboardEntry entries[MAX_PLAYERS_PER_ROOM];
} __attribute__((packed));
```

**Optimization:** Sort in server memory using `std::sort` with **O(n log n)**, n ≤ 10 (negligible: <1ms)

---

## **9. RANKING SYSTEM (ELO-Based)**

### **9.1. Tier Calculation**

**Ranked Points (RP) Formula:**
```cpp
constexpr uint32_t BASE_RP = 1000;
constexpr uint32_t K_FACTOR = 32; // ELO K-factor

// After each ranked game:
uint32_t new_rp = old_rp + K_FACTOR * (actual_win - expected_win);

// Expected win probability:
double expected_win = 1.0 / (1.0 + pow(10.0, (opponent_rp - player_rp) / 400.0));
```

**Tier Thresholds:**
- **Bronze:** 0-1199 RP
- **Silver:** 1200-1599 RP
- **Gold:** 1600-1999 RP
- **Platinum:** 2000-2399 RP
- **Diamond:** 2400+ RP

**Database Storage:**
```sql
ALTER TABLE users ADD COLUMN ranked_points INTEGER DEFAULT 1000;
CREATE INDEX idx_users_tier ON users (ranked_points DESC);
```

---

## **10. ADDITIONAL FEATURES**

### **10.1. Game Replay System**

**Event Sourcing in `game_log`:**
```cpp
struct ReplayEvent {
    uint32_t timestamp_ms; // Relative to game start
    uint32_t question_id;
    uint32_t user_id;
    uint8_t selected_option;
    uint32_t response_time_ms;
};

// Replay reconstruction:
// SELECT * FROM game_log WHERE session_id = ? ORDER BY timestamp;
// Rebuild game state frame-by-frame for playback
```

**Storage Overhead:** 50 bytes per answer × 10 players × 20 questions = **10KB per game** (negligible).

### **10.2. Rate Limiting & DDoS Protection**

```cpp
// server_core/security/RateLimiter.h
class RateLimiter {
    std::unordered_map<uint32_t, TokenBucket> ip_buckets;
    
    bool allowRequest(uint32_t ip) {
        auto& bucket = ip_buckets[ip];
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - bucket.last_update).count();
        
        bucket.tokens += elapsed * (RATE / 1000.0); // RATE = requests/sec
        bucket.tokens = std::min(bucket.tokens, CAPACITY);
        
        if (bucket.tokens >= 1.0) {
            bucket.tokens -= 1.0;
            return true;
        }
        return false;
    }
};
```

**Configuration:** 
- Registration: **3 requests/min/IP**
- Login: **5 requests/min/IP**
- Game actions: **20 requests/sec/IP** (bursts allowed)

---

## **11. PERFORMANCE MONITORING & METRICS**

### **11.1. Key Performance Indicators (KPIs)**

| Metric | Target | Measurement Tool |
|--------|--------|------------------|
| Avg latency per message | < 50ms | `std::chrono::high_resolution_clock` |
| P99 latency | < 200ms | Histogram (Prometheus-style) |
| DB write latency | < 10ms | SQLite profile hook |
| CPU usage (peak) | < 70% | `getrusage()` |
| Memory per connection | < 150KB | `malloc_info()` |

**Integration:**
```cpp
// server_core/metrics/MetricsCollector.cpp
void recordMessageLatency(MessageType type, uint32_t latency_ms) {
    metrics[type].count++;
    metrics[type].total_latency += latency_ms;
    metrics[type].p99_histogram.add(latency_ms);
}
```

---

## **12. ERROR HANDLING & RESILIENCE**

### **12.1. Network Error Recovery**

```cpp
// Socket read/write failure handling
void Connection::handleIOError(int error_code) {
    switch (error_code) {
        case ECONNRESET:  // Client crashed
        case EPIPE:       // Client disconnected
            transitionState(ConnectionState::DISCONNECTED);
            cleanupSession();
            break;
            
        case ETIMEDOUT:   // Network partition
            retryCount++;
            if (retryCount > 3) {
                transitionState(ConnectionState::DISCONNECTED);
            } else {
                scheduleRetry(50ms * pow(2, retryCount)); // Exponential backoff
            }
            break;
    }
}
```

**Session Cleanup:** 
- Remove from `active_sessions`
- Decrement `room->current_players`
- Trigger host migration if necessary
- All operations under **RAII** to ensure no leaks

---

## **13. DEPLOYMENT CONFIGURATION**

### **13.1. Server Start-up Sequence**

```bash
# Configuration file: server.conf
listen_port = 9000
max_connections = 1000
db_path = "game_db.db"
log_level = "INFO"
bcrypt_rounds = 12
room_timeout_sec = 300
question_time_limit = 5000  # milliseconds

# Start command
./quiz_server --config server.conf --workers 4
```

**Process Management:**
- **Daemonize:** Fork to background, PID file at `/var/run/quiz_server.pid`
- **Signal Handling:** `SIGTERM` → graceful shutdown (flush DB, close sockets)
- **Log Rotation:** Support `SIGUSR1` to reopen log files

---

**END OF TECHNICAL DOCUMENTATION**