# Phase 3 Build & Test Guide

## 🚀 Quick Start

### Option 1: WSL/Linux Build
```bash
cd client
chmod +x build.sh
./build.sh
```

### Option 2: Manual Build
```bash
cd client
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)
```

## ✅ Verification Checklist

### Step 1: Check for Syntax Errors
```bash
cd client/build
# Should compile without errors, only maybe deprecation warnings
```

### Step 2: Test UI File Generation
```bash
# Verify AUTOUIC generated files:
ls -la client/build/QuizClient_autogen/include/ui_*.h
```

Should show:
- `ui_LoginWindow.h`
- `ui_LobbyWindow.h`
- `ui_CreateRoomDialog.h`

### Step 3: Run Client
```bash
cd client
./run_client.sh
```

### Step 4: Test Complete Flow

#### 4a. Registration
1. Start `run_server.sh` in WSL terminal 1
2. Start `./run_client.sh` in WSL terminal 2
3. Register new account:
   - Email: `test@example.com`
   - Display Name: `Player123`
   - Password: `password123`

#### 4b. Login
1. Switch to Login tab
2. Enter same email/password
3. **Expected**: LoginWindow hides, **LobbyWindow appears**

#### 4c. Lobby Interaction
1. **Expected**: See room list (initially empty or with existing rooms)
2. Click "Refresh" → Should reload room list
3. Click "Create Room" → CreateRoomDialog appears
   - Enter room name: `My Quiz Room`
   - Select mode: `Elimination`
   - Set questions: `10`
   - Click Create
4. **Expected**: Room appears in list
5. Click "Join" on a room → Should show "Room Joined" message
6. List auto-refreshes every 2 seconds

#### 4d. Logout
1. Click "Logout" button
2. **Expected**: LoginWindow reappears

## 🐛 Troubleshooting

### Build Fails: "ui_LobbyWindow.h not found"
- **Cause**: AUTOUIC didn't run properly
- **Fix**: Delete build dir, rebuild:
  ```bash
  cd client && rm -rf build && mkdir build && cd build
  cmake .. && cmake --build . -- -j$(nproc)
  ```

### Runtime: "LobbyWindow: No such file"
- **Cause**: #include path wrong
- **Fix**: Verify in CreateRoomDialog.cpp:
  ```cpp
  #include "LobbyWindow.h"  // Not "LobbyWindow/LobbyWindow.h"
  ```

### Connection Error
- **Cause**: Server not running
- **Fix**: Start server in WSL:
  ```bash
  cd /mnt/c/Users/Admin/NetworkProgramming/server/build
  ./server
  ```

### No Rooms Appear
- **Cause**: Server has no rooms, or C2S_LIST_ROOMS_REQ failed
- **Fix**: 
  1. Click "Create Room" in LobbyWindow
  2. Fill in dialog and click Create
  3. Room should appear after 2s auto-refresh

## 📊 Expected Output

### Successful Build
```
[100%] Linking CXX executable QuizClient
[100%] Built target QuizClient
=== Build successful! ===
Run with: ./run_client.sh
```

### Successful Run
```
Connected to server
[LoginWindow displays]
→ Register → Login → [LobbyWindow displays]
[Room list appears with table]
```

## 🧪 Network Messages (Debug)

Enable debug output in NetworkManager to see sent/received messages:

### In NetworkManager.cpp constructor:
```cpp
// Add debug logging (optional)
// qDebug() << "Sending C2S_LIST_ROOMS_REQ";
```

## 📝 Files Modified/Created

### New Files (Phase 3)
- `client/src/ui/LobbyWindow.ui` - Main lobby interface
- `client/src/ui/LobbyWindow.h` - Room list management
- `client/src/ui/LobbyWindow.cpp` - Room display + join logic
- `client/src/ui/CreateRoomDialog.ui` - Room creation dialog
- `client/src/ui/CreateRoomDialog.h` - Dialog logic
- `client/src/ui/CreateRoomDialog.cpp` - Room creation handler
- `client/build.sh` - Build script
- `client/PHASE3.md` - Feature documentation

### Modified Files
- `client/CMakeLists.txt` - Added Phase 3 sources
- `client/src/ui/LoginWindow.h` - Added LobbyWindow forward decl
- `client/src/ui/LoginWindow.cpp` - Added navigation to LobbyWindow

## ✨ Next Phase (Phase 4)

After Phase 3 is tested:
1. Create RoomWindow (pre-game lobby with player list)
2. Add ready button + game countdown
3. Implement player list refresh
4. Handle game start transition

## 💾 Git Workflow

```bash
# After testing completes successfully:
cd client
git add -A
git commit -m "Phase 3: Lobby window with room list, create room dialog, and navigation"
git push origin client/phase-3
```

## 🎯 Success Criteria

- ✅ LoginWindow → LobbyWindow navigation works
- ✅ Room list displays in table format
- ✅ Auto-refresh every 2 seconds
- ✅ Create room dialog validates input
- ✅ Join room sends request and shows confirmation
- ✅ Logout returns to LoginWindow
- ✅ No compilation errors
- ✅ No runtime crashes
