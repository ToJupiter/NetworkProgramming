# Phase 3: Architecture Diagrams

## 📊 Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    QUIZ GAME CLIENT                         │
└─────────────────────────────────────────────────────────────┘

START
  ↓
┌──────────────────────┐
│  LoginWindow.ui      │
├──────────────────────┤
│ ┌──────────────────┐ │
│ │ Login Tab        │ │
│ │ Register Tab     │ │
│ └──────────────────┘ │
│ [Login] [Register]   │
└──────────────────────┘
  ↓ (C2S_REGISTER_REQ)
  ↓ (C2S_LOGIN_REQ)
  ↓ onLoginResponse(SUCCESS)
  ↓
┌──────────────────────────────────────────────┐
│         LobbyWindow.ui                       │
├──────────────────────────────────────────────┤
│ Welcome, Player123  [↻][+][📊][🚪]          │
├──────────────────────────────────────────────┤
│ Available Rooms                              │
├──────┬──────────┬──────┬──────┬──────┬──────┤
│ ID   │ Name     │ Plrs │ Mode │ Stat │ Act  │
├──────┼──────────┼──────┼──────┼──────┼──────┤
│ 1    │ Quiz A   │ 3/8  │ Elim │ Wait │[Join]│
│ 2    │ Quiz B   │ 5/10 │ Scor │ Wait │[Join]│
└──────┴──────────┴──────┴──────┴──────┴──────┘
  │ ↻ Auto-refresh every 2 seconds
  │   (C2S_LIST_ROOMS_REQ)
  │
  ├─→ [+] Create Room Button
  │     ↓
  │     ┌─────────────────────────────────────┐
  │     │  CreateRoomDialog.ui (Modal)        │
  │     ├─────────────────────────────────────┤
  │     │ Room Name: [_____________]          │
  │     │ Game Mode: [Elimination ▼]          │
  │     │ Questions: [10 ▲▼]                  │
  │     │ [Error message area]                │
  │     │           [Create][Cancel]          │
  │     └─────────────────────────────────────┘
  │     (C2S_CREATE_ROOM_REQ)
  │
  ├─→ [Join] Button on Room Row
  │     ↓ (C2S_JOIN_ROOM_REQ)
  │     ↓ onJoinRoomResponse(SUCCESS)
  │     ⏳ TODO: Phase 4 - RoomWindow
  │
  ├─→ [📊] Stats Button
  │     → Shows: "Coming in Phase 5"
  │
  └─→ [🚪] Logout Button
        ↓ Confirmation dialog
        ↓ SessionState.clear()
        ↓
      ┌──────────────────────┐
      │  LoginWindow.ui      │ (return to login)
      └──────────────────────┘
```

## 🔌 Class Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                      Network Layer                           │
├──────────────────────────────────────────────────────────────┤
│  NetworkManager (Singleton)                                  │
│  ────────────────────────────────────────────────────────    │
│  - socket: QTcpSocket                                       │
│  - buffer: QByteArray                                       │
│  ────────────────────────────────────────────────────────    │
│  Signals:                                                    │
│    ↳ listRoomsResponse(count, rooms)                        │
│    ↳ createRoomResponse(code, roomInfo)                     │
│    ↳ joinRoomResponse(code, roomInfo, ...)                  │
│    ↳ networkError(error)                                    │
│  Methods:                                                    │
│    ↳ sendListRooms()                                        │
│    ↳ sendCreateRoom(name, mode, qcount)                     │
│    ↳ sendJoinRoom(roomId)                                   │
└──────────────────────────────────────────────────────────────┘
         ↑              ↑                    ↑
         │              │                    │
        uses           uses                 uses
         │              │                    │
┌────────────────┐  ┌─────────────────┐  ┌──────────────────┐
│  LobbyWindow   │  │CreateRoomDialog │  │  SessionState    │
│  (QMainWindow) │  │   (QDialog)     │  │  (Singleton)     │
├────────────────┤  ├─────────────────┤  ├──────────────────┤
│ - tblRooms     │  │ - edtRoomName   │  │ - userId         │
│ - btnCreate    │  │ - cmbGameMode   │  │ - displayName    │
│ - btnJoin      │  │ - spinQuestions │  │ - email          │
│ - refreshTimer │  │                 │  │ - currentRoomId  │
│ - cachedRooms  │  │ Methods:        │  │ - authenticated  │
│ - lobbyWindow* │  │ - validateInput │  │                  │
│                │  │ - onCreateClick │  │ Methods:         │
│ Methods:       │  │                 │  │ - setUserId()    │
│ - setupUI()    │  │ Signals:        │  │ - getUserId()    │
│ - populateTable│  │ - (inherited    │  │ - setCurrentRoom │
│ - onRefresh()  │  │   QDialog)      │  │ - clear()        │
│ - onJoinRoom() │  │                 │  └──────────────────┘
│ - onCreateRoom │  │                 │
│ - onRefresh    │  │                 │
│   Timer()      │  │                 │
└────────────────┘  └─────────────────┘
         ↑                    ↑
         └────────────────────┘
          ↓ parent
         Connected to LoginWindow
```

## 🔄 Sequence Diagram (Auto-Refresh)

```
┌────────────┐                ┌────────────────┐               ┌──────────┐
│ LobbyWindow│                │ NetworkManager │               │  Server  │
└────────────┘                └────────────────┘               └──────────┘
      │                              │                              │
      │  startAutoRefresh()          │                              │
      ├──────────────┐               │                              │
      │  QTimer(2s)  │               │                              │
      └──────────────┘               │                              │
      │  (every 2s)                  │                              │
      │  timeout()                   │                              │
      ├─────────────────────────────→│                              │
      │  sendListRooms()             │  C2S_LIST_ROOMS_REQ          │
      │                              ├─────────────────────────────→│
      │                              │                              │
      │                              │  S2C_LIST_ROOMS_RSP          │
      │                              │←─────────────────────────────┤
      │  listRoomsResponse()         │                              │
      │←─────────────────────────────┤                              │
      │  onListRoomsResponse()       │                              │
      ├──────────────┐               │                              │
      │  populate    │               │                              │
      │  table       │               │                              │
      └──────────────┘               │                              │
      │  (repeat every 2s)           │                              │
```

## 🔄 Sequence Diagram (Create Room)

```
┌────────────┐    ┌──────────────┐    ┌────────────────┐    ┌──────────┐
│ LobbyWindow│    │ CreateRoom   │    │ NetworkManager │    │  Server  │
│            │    │   Dialog     │    │                │    │          │
└────────────┘    └──────────────┘    └────────────────┘    └──────────┘
      │                   │                    │                  │
      │ [Create Room]     │                    │                  │
      ├──────────────────→│                    │                  │
      │                   │ validateInput()    │                  │
      │                   ├──────────────┐     │                  │
      │                   │ [if valid]   │     │                  │
      │                   └──────────────┘     │                  │
      │                   │                    │                  │
      │                   │  sendCreateRoom()  │                  │
      │                   ├───────────────────→│                  │
      │                   │                    │ C2S_CREATE_      │
      │                   │                    │ ROOM_REQ         │
      │                   │                    ├─────────────────→│
      │                   │                    │                  │
      │                   │                    │ S2C_CREATE_      │
      │                   │                    │ ROOM_RSP         │
      │                   │                    │←─────────────────┤
      │                   │                    │                  │
      │ createRoomResponse│                    │                  │
      │←──────────────────┼────────────────────┤                  │
      │                   │                    │                  │
      │                   │ [Show Success]     │                  │
      │                   ├──────────────┐     │                  │
      │                   │ [Close Dialog]     │                  │
      │                   └──────────────┘     │                  │
      │  onRefresh()      │                    │                  │
      ├──────────────────────────────────────→│                  │
      │ [Trigger List Refresh]                │                  │
```

## 🔄 Sequence Diagram (Join Room)

```
┌────────────┐                ┌────────────────┐               ┌──────────┐
│ LobbyWindow│                │ NetworkManager │               │  Server  │
└────────────┘                └────────────────┘               └──────────┘
      │                              │                              │
      │ [Join Button Clicked]        │                              │
      │ onRoomTableItemClicked()     │                              │
      ├──────────────────────────────→│                              │
      │  sendJoinRoom(roomId)        │                              │
      │                              │ C2S_JOIN_ROOM_REQ            │
      │                              ├─────────────────────────────→│
      │                              │                              │
      │                              │  S2C_JOIN_ROOM_RSP           │
      │                              │  (code, roomInfo, players)   │
      │                              │←─────────────────────────────┤
      │                              │                              │
      │  joinRoomResponse()          │                              │
      │←─────────────────────────────┤                              │
      │  onJoinRoomResponse()        │                              │
      ├──────────────────┐           │                              │
      │  if SUCCESS:     │           │                              │
      │  - Store roomId  │           │                              │
      │  - Show confirm  │           │                              │
      │  - Refresh list  │           │                              │
      └──────────────────┘           │                              │
      │                              │                              │
      │ TODO: Phase 4                │                              │
      │ Transition to                │                              │
      │ RoomWindow                   │                              │
```

## 📊 Data Flow Diagram

```
                    ┌─────────────────────────────┐
                    │    Server Database          │
                    │    (SQLite)                 │
                    │  - Rooms Table              │
                    │  - Players Table            │
                    │  - Questions Table          │
                    └─────────────────────────────┘
                              ↑ ↓
                    ┌─────────────────────────────┐
                    │   Server (C++)              │
                    │  (NetworkManager backend)   │
                    │  - Room Manager             │
                    │  - User Manager             │
                    └─────────────────────────────┘
                              ↑ ↓
                    ┌─────────────────────────────┐
                    │   Binary Protocol           │
                    │  (6-byte header + body)     │
                    │  Network byte order (htonl) │
                    └─────────────────────────────┘
                              ↑ ↓
                    ┌─────────────────────────────┐
                    │   Network Socket (QTcpSocket)
                    │   (Port 10002, localhost)   │
                    └─────────────────────────────┘
                              ↑ ↓
┌──────────────────────────────────────────────────────────┐
│              CLIENT (Qt5 Application)                   │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────────┐       ┌─────────────────────┐    │
│  │  LobbyWindow     │       │  CreateRoomDialog   │    │
│  │  (QTableWidget)  │       │  (QLineEdit, etc.)  │    │
│  └────────┬─────────┘       └────────┬────────────┘    │
│           │                          │                 │
│           └──────────┬───────────────┘                 │
│                      ↓                                 │
│              ┌─────────────────────┐                   │
│              │  SessionState       │                   │
│              │  (user info cache)  │                   │
│              └─────────────────────┘                   │
│                      ↑                                 │
│                      │                                 │
│              ┌─────────────────────┐                   │
│              │  NetworkManager     │                   │
│              │  (QTcpSocket)       │                   │
│              │  (Singleton)        │                   │
│              └─────────────────────┘                   │
└──────────────────────────────────────────────────────────┘
```

## 🎯 State Machine

```
            ┌────────────────────┐
            │   START APP        │
            └──────────┬─────────┘
                       ↓
            ┌────────────────────┐
            │  LoginWindow       │
            │  (Register/Login)  │
            └──────────┬─────────┘
                       │
                  [SUCCESS]
                       ↓
            ┌────────────────────┐
            │  LobbyWindow       │  ← YOU ARE HERE (Phase 3)
            │  (Room List)       │
            └──────────┬─────────┘
                   /   |   \
               [+] /    |    \ [Join]
              /   |     |     \
             /    |     │      \
            ↓     ↓     │       ↓
       [Dialog]  [List] │   [TODO: Phase 4]
           |           │        |
           ↓           ↓        ↓
      [Create]    [Refresh]  RoomWindow
         Room       (auto)  (pre-game)
                            │
                            ├─→ [Ready]
                            ├─→ [Chat]
                            ├─→ [Start Game]
                            │
                            ↓
                        GameWindow (Phase 5)
                        (questions & answers)
                            │
                            ↓
                        ResultsWindow (Phase 6)
                        (scoreboard)
                            │
                       [Play Again]
                            ↓
                        LobbyWindow (loop)
         _______________|
        │               │
        └─→ [Logout] ──→ LoginWindow
```

## 📡 Protocol Message Exchange

```
CLIENT                                    SERVER
   │                                         │
   │──── C2S_REGISTER_REQ ────────────────→ │
   │                                         │
   │←─── S2C_REGISTER_RSP ─────────────────│
   │                                         │
   │ [Switch to Login Tab]                  │
   │                                         │
   │──── C2S_LOGIN_REQ ────────────────────→ │
   │                                         │
   │←─── S2C_LOGIN_RSP (userId, name) ────│
   │                                         │
   │ [Show LobbyWindow]                     │
   │                                         │
   │ [QTimer every 2 seconds]               │
   │──── C2S_LIST_ROOMS_REQ ───────────────→ │
   │←─── S2C_LIST_ROOMS_RSP (rooms) ──────│
   │ [Update Table]                         │
   │                                         │
   │ [User clicks Create Room]              │
   │ [Opens CreateRoomDialog]               │
   │                                         │
   │──── C2S_CREATE_ROOM_REQ ──────────────→ │
   │←─── S2C_CREATE_ROOM_RSP ──────────────│
   │ [Room appears in list]                 │
   │                                         │
   │ [User clicks Join]                     │
   │──── C2S_JOIN_ROOM_REQ ────────────────→ │
   │←─── S2C_JOIN_ROOM_RSP ────────────────│
   │ [TODO: Phase 4 - Show RoomWindow]      │
   │                                         │
```

---

These diagrams show:
1. **Flow Diagram**: User journey through the application
2. **Class Diagram**: Component relationships
3. **Sequence Diagrams**: Message exchange timing
4. **Data Flow**: How data moves through the system
5. **State Machine**: Application states and transitions
6. **Protocol Exchange**: Network message flow

All integrated with your existing Phase 1 architecture!
