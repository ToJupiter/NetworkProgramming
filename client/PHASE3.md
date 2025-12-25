# Phase 3: Lobby Window Implementation

## 📋 Overview
Phase 3 adds the main game lobby interface where users can:
- View available game rooms in a table
- Refresh room list (auto-refresh every 2 seconds)
- Create new rooms with custom settings
- Join existing rooms
- View player statistics (stub)
- Logout

## 🎯 New Components

### 1. **LobbyWindow** (Main Lobby Interface)
- **File**: `src/ui/LobbyWindow.ui`, `LobbyWindow.h`, `LobbyWindow.cpp`
- **Features**:
  - Room list table (ID, Name, Players, Mode, Status)
  - Auto-refresh timer (2 seconds)
  - Join button for each room
  - Create room dialog launcher
  - Stats and logout buttons
  
### 2. **CreateRoomDialog** (Room Creation)
- **File**: `src/ui/CreateRoomDialog.ui`, `CreateRoomDialog.h`, `CreateRoomDialog.cpp`
- **Features**:
  - Room name input (1-32 chars)
  - Game mode selector (Elimination/Scoring)
  - Question count spinner (5-20)
  - Max players spinner (2-50)
  - Input validation

### 3. **Navigation**
- LoginWindow → LobbyWindow on successful login
- LobbyWindow displays when `onLoginResponse(SUCCESS)` received

## 🔌 Network Integration

### Signals Used
- `NetworkManager::listRoomsResponse()` → Populate room table
- `NetworkManager::createRoomResponse()` → Show creation feedback
- `NetworkManager::joinRoomResponse()` → Handle room join

### Messages Sent
- `C2S_LIST_ROOMS_REQ` (every 2 seconds auto-refresh)
- `C2S_CREATE_ROOM_REQ` (from CreateRoomDialog)
- `C2S_JOIN_ROOM_REQ` (on Join button click)

## 🏗️ Architecture Changes

### CMakeLists.txt Updates
```cmake
# Added Phase 3 sources
src/ui/LobbyWindow.cpp
src/ui/CreateRoomDialog.cpp

# Added UI files
src/ui/LobbyWindow.ui
src/ui/CreateRoomDialog.ui
```

### LoginWindow Enhancement
```cpp
// After successful login:
- Hide LoginWindow
- Create and show LobbyWindow
- SessionState populated with user info
```

## 📊 Room Display Format

| Room ID | Room Name | Players | Mode | Status | Action |
|---------|-----------|---------|------|--------|--------|
| 1 | Trivia Night | 3/8 | Elimination | Waiting | [Join] |
| 2 | Weekly Quiz | 5/10 | Scoring | Waiting | [Join] |

## 🎮 Game Mode Definitions
- **Elimination (0)**: Players eliminated on wrong answer (last one standing wins)
- **Scoring (1)**: Points awarded for speed + correctness

## ✅ Build & Test

### Build
```bash
cd client
chmod +x build.sh
./build.sh
```

### Run
```bash
./run_client.sh
```

### Test Flow
1. Register new account
2. Login with credentials
3. See LobbyWindow with room list
4. Create a new room
5. Join a room (auto-connects)

## 🔄 Next Phase (Phase 4)
- **RoomWindow**: Pre-game lobby (player list, ready button, game countdown)
- User joins room → RoomWindow displays
- Ready button toggles player ready status
- Host can start game when all ready

## 📝 Notes
- Room list refreshes automatically every 2 seconds (can be optimized with real-time updates later)
- Create room dialog is modal (blocks interaction until closed)
- Join room success shows confirmation and refreshes list
- Network errors display in red error label
