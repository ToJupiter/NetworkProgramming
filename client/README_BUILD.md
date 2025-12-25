# Client Build Instructions (Linux/WSL)

## Problem: Symbol lookup error with snap Qt

The error `symbol lookup error: /snap/core20/.../libpthread.so.0: undefined symbol: __libc_pthread_init` occurs when the binary links against snap-packaged Qt libraries that conflict with system glibc.

## Solution: Use system Qt packages

### 1. Remove conflicting snap Qt (if installed via snap)
```bash
snap list | grep qt
# If Qt snap packages exist, remove them:
sudo snap remove --purge qt5-core-20  # example
```

### 2. Install Qt5 from Ubuntu repositories
```bash
sudo apt update
sudo apt install -y qtbase5-dev qt5-qmake qtbase5-dev-tools libqt5network5
```

### 3. Clean and rebuild
```bash
cd ~/NetworkProgramming/client
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . -j
```

### 4. Run the client
```bash
./QuizClient
```

## Alternative: Force system library path

If you cannot remove snap Qt, try forcing library search order:

```bash
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
./QuizClient
```

## Expected behavior

- Client should launch Qt GUI window
- Connection attempt to 127.0.0.1:10002
- If server not running: connection error dialog
- If server running: Login/Register window appears

## Testing without GUI (X server)

If running in pure WSL without X server:
1. Install VcXsrv or X410 on Windows
2. Set DISPLAY:
```bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
# or for localhost X server:
export DISPLAY=:0
```

## Server setup (parallel terminal)

```bash
cd ~/NetworkProgramming/server/build
cmake .. && cmake --build . -j
./server 10002
```

Then run client in another terminal.
