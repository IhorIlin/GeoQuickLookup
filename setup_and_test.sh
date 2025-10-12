#!/bin/bash
set -euo pipefail

echo "Checking environment..."

# Function to ensure a tool exists or install if possible
install_tool() {
    local tool="$1"

    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "'$tool' not found."

        if command -v brew >/dev/null 2>&1; then
            echo "Installing '$tool' with Homebrew..."
            brew install -q "$tool"
        elif command -v apt-get >/dev/null 2>&1; then
            echo "Installing '$tool' with apt..."
            sudo apt-get update -qq
            sudo apt-get install -y "$tool" >/dev/null
        else
            echo "Cannot auto-install '$tool'. Please install it manually."
            exit 1
        fi
    else
        echo "'$tool' found."
    fi
}

# Check required tools (basic environment)
install_tool python3
install_tool clang++

# Install Python dependencies
echo "Installing Python dependencies..."
python3 -m pip install --upgrade pip -q
python3 -m pip install -q psutil

# --- Handle CMake setup ---
CMAKE_ISO="$HOME/.cmake/cmake-3.0.0/bin/cmake"

if [ -x "$CMAKE_ISO" ]; then
    echo "Found isolated CMake: $CMAKE_ISO"
else
    echo "Isolated CMake not found — installing..."
    if [ -f "./install_cmake.sh" ]; then
        bash ./install_cmake.sh
    else
        echo "Missing install_cmake.sh — cannot continue."
        exit 1
    fi

    if [ ! -x "$CMAKE_ISO" ]; then
        echo "Installation failed — CMake not found at $CMAKE_ISO"
        exit 1
    fi
fi

# --- Build project ---
echo "Building project..."
cd geolocation
rm -rf build
mkdir -p build && cd build

"$CMAKE_ISO" .. > /dev/null
make > /dev/null

# --- Run tests ---
echo "Running tests..."
cd ..
python3 geolocation_test.py --executable ./build/Sample_app --database database.csv

echo ""
echo "All done successfully!"