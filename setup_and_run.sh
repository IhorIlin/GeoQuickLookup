#!/bin/bash
set -euo pipefail

echo "Checking system dependencies..."

# PATH variables
ROOT_DIR=$(pwd)
GEO_DIR="$ROOT_DIR/geolocation"
BUILD_DIR="$GEO_DIR/build"
CSV_GZ="$GEO_DIR/database.csv.gz"
CSV_FILE="$GEO_DIR/database.csv"
BIN_FILE="$GEO_DIR/database.bin"
PREPROCESS_BIN="$BUILD_DIR/Geo_processor"
APP_BIN="$BUILD_DIR/Sample_app"
TEST_SCRIPT="$GEO_DIR/geolocation_test.py"

# OS detection
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  INSTALL_CMD="sudo apt-get install -y"
  UPDATE_CMD="sudo apt-get update -qq"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  INSTALL_CMD="brew install"
  UPDATE_CMD="brew update"
else
  echo "Unsupported OS: $OSTYPE"
  exit 1
fi

# Install helper
install_tool() {
  local tool="$1"
  local pkg="${2:-$1}"
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "Installing $tool..."
    $UPDATE_CMD
    $INSTALL_CMD $pkg
  else
    echo "$tool is already installed."
  fi
}

# Check required tools
install_tool cmake
install_tool make
install_tool python3
install_tool pip3 python3-pip
install_tool clang++ g++
install_tool gunzip gzip

# Check python module
if ! python3 -c "import psutil" 2>/dev/null; then
  echo "Installing psutil..."
  python3 -m pip install --user psutil
fi

# unzip database if needed
if [ ! -f "$CSV_FILE" ]; then
  if [ -f "$CSV_GZ" ]; then
    echo "Unzipping $CSV_GZ..."
    gunzip -k "$CSV_GZ"
  else
    echo "Error: database.csv or database.csv.gz not found."
    exit 1
  fi
fi

# Build project
echo "Building project..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. >/dev/null
make
cd "$ROOT_DIR"

# Create binary database
echo "Creating binary database..."
"$PREPROCESS_BIN" "$CSV_FILE" "$BIN_FILE"

# Running tests
echo "Running tests..."
python3 "$TEST_SCRIPT" --executable "$APP_BIN" --database "$BIN_FILE"

echo "Done."