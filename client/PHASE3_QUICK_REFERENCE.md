# Phase 3: Quick Reference Guide

## 🎯 What You Get

| Feature | Details |
|---------|---------|
| **Lobby Window** | Main window showing available game rooms |
| **Room List Table** | 6 columns: ID, Name, Players, Mode, Status, Action |
| **Auto-Refresh** | Updates every 2 seconds automatically |
| **Create Room** | Modal dialog to create new game room |
| **Join Room** | Click "Join" button on any room |
| **Navigation** | Login → Lobby → Logout returns to Login |
| **User Info** | Display name shown in welcome label |

## 📦 Files Created

```
LobbyWindow.ui               Room list UI design
LobbyWindow.h/cpp           Room management class
CreateRoomDialog.ui         Dialog UI design
CreateRoomDialog.h/cpp      Room creation dialog class
```

## 🔧 Files Modified

```
CMakeLists.txt              Added Phase 3 sources
LoginWindow.h/cpp           Added LobbyWindow navigation
```

## 📋 Documentation Created

```
PHASE3.md                   Feature overview
PHASE3_BUILD.md             Build & test instructions
PHASE3_SUMMARY.md           Implementation details
PHASE3_CHECKLIST.md         Verification checklist
(This file)
```

## 🚀 Build & Run

### Build (Option A - Automated)
```bash
cd client
chmod +x build.sh
./build.sh
```

### Build (Option B - Manual)
```bash
cd client/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)
```

### Run
```bash
cd client
./run_client.sh
```

## 🧪 Test Flow

1. **Register**: email, displayName, password
2. **Login**: Use registered credentials
3. **See Lobby**: Room list appears (may be empty)
4. **Create Room**: 
   - Click "Create Room"
   - Fill: Name, Mode, Questions
   - Click "Create"
5. **Join Room**: Click "Join" on a room
6. **Logout**: Click "Logout" button

## ⏱️ Auto-Refresh

Room list updates automatically every **2 seconds** while viewing lobby.

## 🎮 Game Modes

| Mode | Details |
|------|---------|
| **Elimination** | Players eliminated on wrong answer (last one wins) |
| **Scoring** | Points awarded for speed + correct answers |

## 📊 Room Status

| Status | Meaning |
|--------|---------|
| **Waiting** | Game not started, accepting players |
| **In Game** | Game in progress |
| **Finished** | Game ended |

## 🔌 Network Messages

| Message | Direction | Purpose |
|---------|-----------|---------|
| `C2S_LIST_ROOMS_REQ` | Client→Server | Get room list |
| `S2C_LIST_ROOMS_RSP` | Server→Client | Room list response |
| `C2S_CREATE_ROOM_REQ` | Client→Server | Create new room |
| `S2C_CREATE_ROOM_RSP` | Server→Client | Creation result |
| `C2S_JOIN_ROOM_REQ` | Client→Server | Join a room |
| `S2C_JOIN_ROOM_RSP` | Server→Client | Join result |

## 💾 State Stored

**SessionState contains:**
- userId (from server)
- displayName (from server)
- email (user input)
- currentRoomId (when in room)
- authenticated (true after login)

## 🐛 Common Issues & Fixes

### Build Error: "ui_LobbyWindow.h not found"
```bash
# Delete build, rebuild from scratch
cd client && rm -rf build
mkdir build && cd build
cmake .. && cmake --build . -- -j$(nproc)
```

### No Rooms Appear
1. Check server is running: `./server` in WSL
2. Create a room via dialog
3. Wait 2 seconds for auto-refresh

### Connection Error
- Ensure server running on localhost:10002
- Check firewall not blocking port

### Room Join Failed
- Check server room still exists
- Check player count not exceeded
- Check game not already in progress

## 📊 UI Components

### Buttons
| Button | Action |
|--------|--------|
| Refresh | Manual room list refresh |
| Create Room | Open creation dialog |
| Stats | Show player statistics (stub) |
| Logout | Close lobby, return to login |
| Join | Join selected room |

### Table Columns
| Column | Content |
|--------|---------|
| Room ID | Numeric room identifier |
| Room Name | User-specified room name |
| Players | Current/Max (e.g., "3/8") |
| Mode | Elimination or Scoring |
| Status | Waiting, InGame, or Finished |
| Action | Join button |

## ⚙️ Configuration

### Room Creation Constraints
- **Room Name**: 1-32 characters
- **Question Count**: 5-20 questions
- **Game Mode**: Elimination (0) or Scoring (1)

### Auto-Refresh
- **Interval**: 2000 milliseconds (2 seconds)
- **Can be disabled**: Call `stopAutoRefresh()`
- **Can be resumed**: Call `startAutoRefresh()`

## 🎯 Phase 3 → Phase 4

When ready for **Phase 4 (RoomWindow)**:

1. Create `RoomWindow.ui` + `RoomWindow.h/cpp`
2. Show RoomWindow after successful `joinRoomResponse(SUCCESS)`
3. Display player list with ready status
4. Add "Ready" toggle button
5. Show game countdown when host starts

## 📈 Scalability Notes

**Current Design:**
- Refresh every 2 seconds (polling)
- Suitable for up to 50-100 rooms

**Future Optimization:**
- Real-time updates via server push
- Only refresh when rooms change
- Pagination for large room lists

## 🔐 Security Considerations

✅ **Implemented:**
- Password protected login
- Session state isolated per user
- Network messages validated by server

⏳ **Future:**
- Room password protection
- Admin controls
- Player ban system

## 📚 Key Classes

### LobbyWindow (QMainWindow)
Main lobby interface. Member: `NetworkManager*, SessionState*, QTimer*, QVector<RoomInfo>`

### CreateRoomDialog (QDialog)
Modal dialog for room creation. Member: `NetworkManager*`

### NetworkManager (Singleton QObject)
Handles all network communication. Signals emit room list & responses.

### SessionState (Singleton)
Stores authenticated user info. Access via `SessionState::instance()`

## 🎓 Learning Points

**Qt Concepts Covered:**
- QMainWindow + QDialog
- QTableWidget with dynamic rows
- QTimer for periodic tasks
- Signals/Slots for async operations
- Modal dialogs (blocking parent)
- Input validation
- Error handling with QMessageBox

**C++ Patterns:**
- Singleton pattern (NetworkManager, SessionState)
- Signal/Slot pattern (Qt MOC)
- Parent/Child ownership (memory management)
- Forward declarations (avoid circular includes)

---

**Phase 3 Status**: ✅ **COMPLETE & TESTED**

Ready to move to **Phase 4: RoomWindow** when needed.
