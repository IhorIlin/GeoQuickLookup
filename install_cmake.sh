#!/bin/bash
set -euo pipefail

CMAKE_VERSION="3.0.0"
BASE_DIR="$HOME/.cmake"
BUILD_DIR="$BASE_DIR/tmp_build"
INSTALL_DIR="$BASE_DIR/cmake-$CMAKE_VERSION"
ARCHIVE_NAME="cmake-$CMAKE_VERSION.tar.gz"
CMAKE_URL="https://cmake.org/files/v3.0/${ARCHIVE_NAME}"

echo "📦 Installing CMake $CMAKE_VERSION in isolated directory..."

# Check if already installed
if [ -x "$INSTALL_DIR/bin/cmake" ]; then
  echo "✅ CMake $CMAKE_VERSION already installed in: $INSTALL_DIR"
  echo "👉 Use it with:"
  echo "   $INSTALL_DIR/bin/cmake --version"
  exit 0
fi

# Prepare directory
mkdir -p "$BASE_DIR"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Download
echo "⬇️ Downloading CMake..."
if command -v curl >/dev/null 2>&1; then
  curl -fsSL -o "$ARCHIVE_NAME" "$CMAKE_URL"
elif command -v wget >/dev/null 2>&1; then
  wget -q -O "$ARCHIVE_NAME" "$CMAKE_URL"
else
  echo "❌ Please install curl or wget first."
  exit 1
fi

# Extract
echo "📂 Extracting..."
tar -xzf "$ARCHIVE_NAME"
cd "cmake-$CMAKE_VERSION"

# Detect CPU cores
CORES=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
echo "⚙️ Building with $CORES parallel jobs..."

# Build and install
./bootstrap --prefix="$INSTALL_DIR" > /dev/null 2>&1
make -j"$CORES" > /dev/null 2>&1
make install > /dev/null 2>&1

# Cleanup
cd "$BASE_DIR"
rm -rf "$BUILD_DIR"

# Final info
echo ""
echo "✅ CMake $CMAKE_VERSION installed successfully!"
echo "📁 Location: $INSTALL_DIR"
echo ""
echo "👉 To use it in this terminal session, run:"
echo "   export PATH=\"$INSTALL_DIR/bin:\$PATH\""
echo ""
echo "👉 To make it permanent, add that line to your ~/.bashrc or ~/.zshrc"
echo ""
echo "✅ Verify installation:"
echo "   $INSTALL_DIR/bin/cmake --version"
