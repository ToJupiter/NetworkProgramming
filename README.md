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
