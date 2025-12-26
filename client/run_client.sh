#!/bin/bash
# QuizClient Launcher - Runs client with clean environment to avoid snap conflicts

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLIENT_BIN="$SCRIPT_DIR/build/QuizClient"

# Check if binary exists
if [ ! -f "$CLIENT_BIN" ]; then
    echo "Error: QuizClient not found at $CLIENT_BIN"
    echo "Please build first: cd build && cmake .. && cmake --build . -j"
    exit 1
fi

# Clean environment, keep only essential variables
env -i \
    HOME="$HOME" \
    USER="$USER" \
    DISPLAY="${DISPLAY:-:0}" \
    XAUTHORITY="${XAUTHORITY:-$HOME/.Xauthority}" \
    XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}" \
    PATH="/usr/bin:/bin:/usr/local/bin" \
    LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu:/lib/x86_64-linux-gnu" \
    QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-xcb}" \
    "$CLIENT_BIN" "$@"
