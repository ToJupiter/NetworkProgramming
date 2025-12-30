# Quiz Game — Quick Start (Linux)

🎯 **Purpose** — Build and run the server and the Qt client locally on Linux (Ubuntu/Debian). This README has concise dependency, build, DB init, run, and quick tests (replay & quit behavior).

---

## Requirements

- Linux (tested on Ubuntu)
- C++ toolchain: gcc/g++, make, cmake (≥ 3.10)
- Qt (prefer Qt6, Qt5 fallback) — dev packages
- Python: optional for scripts

Recommended apt packages (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install -y build-essential cmake git
# For client (Qt dev)
sudo apt install -y qtbase5-dev qt5-qmake qtbase5-dev-tools
```

Note: `client/CMakeLists.txt` prefers Qt6 when available and falls back to Qt5.

---

## Project layout (important files)

- `server/` — Server implementation (CMake)
	- `server/src/main.cpp` — server entrypoint (default port **10002**) 
	- `sql_schema/database_init.sql` — DB schema
- `client/` — Qt client (CMake)
- `common/protocol.h` — protocol & shared message structs

---

## Initialize database

The server uses a local SQLite DB file `server/game_db.db` in the server working directory.

From the repository root:

```bash
# create DB file and schema (run from repo root)
sqlite3 server/game_db.db < sql_schema/database_init.sql
# optionally add sample data
# sqlite3 server/game_db.db < sql_schema/sample_data.sql
```

> Tip: `server/DatabaseManager` opens `game_db.db` in the server working directory.

---

## Build & run server ✅

```bash
cd server
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
# Run (default port 10002)
./server
# or to use a different port:
# ./server 10003
```

When server starts it will create/open `server/game_db.db` and apply PRAGMA settings.

---

## Build & run client ✅

```bash
cd client
./build.sh   # or: mkdir build && cd build && cmake .. && cmake --build . -j$(nproc)
./run_client.sh  # launches the Qt client with a clean env
```

Notes:
- The client will auto-detect Qt6 or Qt5 (see `client/CMakeLists.txt`).
- If you get Qt not found errors, install Qt dev packages or point CMake to your Qt installation.

---

## Quick functional checks & testing 💡

- **Replay speed fix**: If replay previously took >30s, rebuild both server and client. The server now sends only the required replay events (no fixed 10k events allocation), and the client parses the streamed events. Test by selecting a session in the UI (Main Menu → Replay) and press Play.

- **Quit / Forfeit**: Client `GameWindow` now has **Quit** button. Confirming will send a forfeit and the server applies an **-100 ranked points** penalty to the user (can't go below 0). You can inspect `users.ranked_points` in `server/game_db.db` to verify.

- **Stay in Room**: After game end, the results window now offers **Stay in Room** to remain in the room and auto-refresh the player list.

---

## Troubleshooting ⚠️

- Build errors about missing Qt: install Qt dev packages or set `CMAKE_PREFIX_PATH` to your Qt install.
- If replays still appear slow: ensure you rebuilt **both** `server` and `client` after pulling changes.
- Database errors on server start: check that `server/game_db.db` exists and that `sql_schema/database_init.sql` was applied.

---

## Notes & References

- Default server port: **10002** (changeable via `./server <port>`)
- DB file: `server/game_db.db`
- If you push these changes, mention in the commit / PR that replay payloads were changed to dynamic size (clients and servers need to be in sync).
