#!/bin/bash
set -euo pipefail

CMAKE_VERSION="3.0.0"
BASE_DIR="$HOME/.cmake"
BUILD_DIR="$BASE_DIR/tmp_build"
INSTALL_DIR="$BASE_DIR/cmake-$CMAKE_VERSION"
ARCHIVE_NAME="cmake-$CMAKE_VERSION.tar.gz"
CMAKE_URL="https://cmake.org/files/v3.0/${ARCHIVE_NAME}"

if ! command -v make >/dev/null 2>&1; then
  echo "Installing required build tools..."
  if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update -qq
    sudo apt-get install -y build-essential >/dev/null
  elif command -v dnf >/dev/null 2>&1; then
    sudo dnf groupinstall -y "Development Tools" >/dev/null
  elif command -v xcode-select >/dev/null 2>&1; then
    xcode-select --install
  else
    echo "Cannot install build tools automatically. Please install them manually."
    exit 1
  fi
fi

echo "Installing CMake $CMAKE_VERSION in isolated directory..."

# Check if already installed
if [ -x "$INSTALL_DIR/bin/cmake" ]; then
  echo "CMake $CMAKE_VERSION already installed in: $INSTALL_DIR"
  echo "Use it with:"
  echo "$INSTALL_DIR/bin/cmake --version"
  exit 0
fi

# Detect or install GCC 9
echo "Checking compiler..."
if command -v g++-9 >/dev/null 2>&1 && command -v gcc-9 >/dev/null 2>&1; then
  export CC=gcc-9
  export CXX=g++-9
  echo "Found GCC 9 — using it."
else
  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "GCC 9 not found. Installing..."
    sudo apt update -qq
    sudo apt install -y gcc-9 g++-9 make
    export CC=gcc-9
    export CXX=g++-9
  else
    echo "GCC 9 not found, and auto-install is only supported on Linux."
    echo "Please install GCC 8 manually (e.g. with Homebrew)."
  fi
fi

# Prepare directory
mkdir -p "$BASE_DIR"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Download
echo "Downloading CMake..."
if command -v curl >/dev/null 2>&1; then
  curl -fsSL -o "$ARCHIVE_NAME" "$CMAKE_URL"
elif command -v wget >/dev/null 2>&1; then
  wget -q -O "$ARCHIVE_NAME" "$CMAKE_URL"
else
  echo "Please install curl or wget first."
  exit 1
fi

# Extract
echo "Extracting..."
tar -xzf "$ARCHIVE_NAME"
cd "cmake-$CMAKE_VERSION"

# Detect CPU cores
CORES=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
echo "Building with $CORES parallel jobs..."

# Build and install
./bootstrap --prefix="$INSTALL_DIR" > /dev/null 2>&1
make -j"$CORES" > /dev/null 2>&1
make install > /dev/null 2>&1

# Cleanup
cd "$BASE_DIR"
rm -rf "$BUILD_DIR"

# Final info
echo ""
echo "CMake $CMAKE_VERSION installed successfully!"
echo "Location: $INSTALL_DIR"
echo ""
echo "To use it in this terminal session, run:"
echo "   export PATH=\"$INSTALL_DIR/bin:\$PATH\""
echo ""
echo "To make it permanent, add that line to your ~/.bashrc or ~/.zshrc"
echo ""
echo "Verify installation:"
echo "   $INSTALL_DIR/bin/cmake --version"
