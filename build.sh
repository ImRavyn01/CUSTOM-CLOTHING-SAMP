#!/bin/bash
# ============================================================================
#  Build Script for Custom CJ Clothing Plugin (Linux)
#  Builds Server Plugin (.so) only (ASI is Windows-only)
# ============================================================================

echo "============================================"
echo " Custom CJ Clothing Plugin - Build (Linux)"
echo " Version: 1.0.0"
echo "============================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build_linux"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "[ERROR] CMake not found! Install with: sudo apt install cmake"
    exit 1
fi

# Check for g++
if ! command -v g++ &> /dev/null; then
    echo "[ERROR] g++ not found! Install with: sudo apt install build-essential"
    exit 1
fi

# Create build directory
echo "[1/4] Creating build directory..."
mkdir -p "$BUILD_DIR"

# Configure
echo "[2/4] Running CMake configuration..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32"
if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed!"
    echo "Make sure 32-bit libraries are installed:"
    echo "  sudo apt install gcc-multilib g++-multilib"
    exit 1
fi

# Build
echo ""
echo "[3/4] Building project..."
cmake --build . --config Release -- -j$(nproc)
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed!"
    exit 1
fi

echo ""
echo "[4/4] Build completed successfully!"
echo ""
echo "============================================"
echo " OUTPUT FILES:"
echo "============================================"

if [ -f "$OUTPUT_DIR/server/plugins/clothing.so" ]; then
    echo " [OK] Server Plugin:  output/server/plugins/clothing.so"
else
    echo " [--] Server Plugin:  NOT FOUND"
fi

if [ -f "$OUTPUT_DIR/server/include/clothing.inc" ]; then
    echo " [OK] Pawn Include:   output/server/include/clothing.inc"
else
    echo " [--] Pawn Include:   NOT FOUND"
fi

echo ""
echo " NOTE: Client ASI plugin (.asi) is Windows-only."
echo "       Build on Windows for the client plugin."
echo "============================================"
