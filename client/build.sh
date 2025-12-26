#!/bin/bash
set -e

echo "=== Building Phase 3: Lobby Window ==="

# Enter client directory
cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# Configure CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
echo "Building..."
cmake --build . --config Debug -- -j$(nproc)

echo ""
echo "=== Build successful! ==="
echo "Run with: ./run_client.sh"
