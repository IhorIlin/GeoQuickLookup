# Geolocation App — Setup and Usage Guide

This project is a simple geolocation lookup application written in C++.  
It demonstrates how a database can be loaded into memory and queried efficiently.  
A Python-based test script is provided to validate the functionality and measure performance.

---

## Supported Operating Systems

This project supports the following systems:

- **macOS** (Monterey or newer)
- **Linux** (Ubuntu or Debian-based distributions)

> Windows users can try using **WSL (Ubuntu)** for compatibility.

---

## Folder Structure

Your cloned repository should look like this:

```
LL-Ihor-Ilin/
│
├── geolocation/
│   ├── sample_app.cpp
│   ├── CMakeLists.txt
│   ├── geolocation_test.py
│   ├── database.csv
│   └── (other project files)
│
├── install_cmake.sh
├── setup_and_test.sh
└── README.md
```

Both shell scripts (`install_cmake.sh` and `setup_and_test.sh`) are located in the **root folder**,  
at the same level as the `geolocation/` directory and `README.md` file.

---

## Quick Start (Automatic Setup)

If you want to build and test everything automatically, follow these steps:

1. Open your terminal.
2. Clone the repository and navigate into it:
   ```bash
   git clone git@github.com:NordSecurity-Interviews/LL-Ihor-Ilin.git
   cd LL-Ihor-Ilin
   ```
3. Run the setup script:
   ```bash
   bash setup_and_test.sh
   ```

This script will:
- Check if all required tools (CMake, Clang, Python) are installed.
- Automatically install **CMake 3.0.0** locally if needed.
- Install Python dependencies (`psutil`) if missing.
- Build the C++ project.
- Run tests automatically.

---

## Manual Setup (Step-by-Step)

If you prefer to configure everything manually, follow these instructions carefully.

### For Linux (Ubuntu / Debian)

1. **Update your package lists**  
   Open the terminal and run this command to ensure your system has access to the latest packages:
   ```bash
   sudo apt update
   ```

2. **Install the required build tools and dependencies**  
   These include the compiler, build system, and Python support:
   ```bash
   sudo apt install -y build-essential clang make cmake python3 python3-pip python3-psutil
   ```

3. **Verify that tools are installed correctly**  
   Run these commands to check versions and confirm successful installation:
   ```bash
   clang++ --version
   cmake --version
   python3 --version
   ```

4. **Build the C++ project**  
   Create a new build directory (to keep files organized), configure the project using CMake, and compile it:
   ```bash
   cd geolocation
   mkdir -p build && cd build
   cmake ..
   make
   ```

5. **Run the automated tests**  
   Execute the Python test script to check that everything works properly:
   ```bash
   python3 ../geolocation_test.py --executable ./Sample_app --database ../database.csv
   ```

---

### For macOS

1. **Install the Xcode Command Line Tools**  
   These tools include the Clang compiler and other essential utilities.  
   In your terminal, run:
   ```bash
   xcode-select --install
   ```

2. **Install Homebrew (if you don’t already have it)**  
   Homebrew is the package manager for macOS. Run this command in your terminal:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install required dependencies using Homebrew**  
   This installs CMake, Python, and the `psutil` package:
   ```bash
   brew install cmake python
   pip3 install psutil
   ```

4. **Build the C++ project**  
   Create a build directory, run CMake, and compile the application:
   ```bash
   cd geolocation
   mkdir -p build && cd build
   cmake ..
   make
   ```

5. **Run the tests**  
   Use Python to run the test suite:
   ```bash
   python3 ../geolocation_test.py --executable ./Sample_app --database ../database.csv
   ```

---

## Installing CMake 3.0.0 Manually

If your system’s version of CMake is too new or incompatible, you can install version **3.0.0** locally.

1. **Download the CMake 3.0.0 archive**  
   Run one of the following commands depending on which tool you have:
   ```bash
   wget https://cmake.org/files/v3.0/cmake-3.0.0.tar.gz
   ```
   or
   ```bash
   curl -O https://cmake.org/files/v3.0/cmake-3.0.0.tar.gz
   ```

2. **Extract the downloaded archive**  
   ```bash
   tar -xzf cmake-3.0.0.tar.gz
   cd cmake-3.0.0
   ```

3. **Configure and build**  
   Set up the installation directory and compile CMake from source:
   ```bash
   ./bootstrap --prefix=$HOME/.cmake/cmake-3.0.0
   make -j$(getconf _NPROCESSORS_ONLN)
   make install
   ```

4. **Add the new CMake to your PATH**  
   This allows you to use your custom version in any terminal session:
   ```bash
   echo 'export PATH="$HOME/.cmake/cmake-3.0.0/bin:$PATH"' >> ~/.bashrc
   source ~/.bashrc
   ```

5. **Check that it’s installed correctly**  
   ```bash
   cmake --version
   ```

---

### Using the Provided Installer Script

You can skip all the manual steps above and simply run:
```bash
bash install_cmake.sh
```
This script will automatically:
- Download CMake 3.0.0
- Build and install it into `~/.cmake/cmake-3.0.0`
- Clean up temporary files afterward

---

## Verifying the Build

Once everything is installed, you can manually test the app:

```bash
cd geolocation
./build/Sample_app database.csv
```

You should see output similar to:
```
READY
LOAD
OK
LOOKUP 12.12.12.12
US,New York
EXIT
OK
```

If you see this, your environment is configured correctly.

---

## Notes

- The `psutil` Python package is only used by the test script to measure performance metrics.
- All system tools installed through the scripts are isolated and won’t interfere with your system versions.
- Both macOS and Linux users can use the same scripts for setup and testing.