Build and run server code(in a terminal in server/):
```bash
# create build dir and configure
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# build (parallel)
cmake --build . -- -j$(nproc)

# run the produced executable
./server
```

Client Qt:
1. Installation:
``` bash
sudo apt update
sudo apt install -y build-essential cmake qtbase5-dev qt5-qmake qtbase5-dev-tools
```

2. Build and run:
``` bash
cd client
./build.sh
./run_client.sh
```