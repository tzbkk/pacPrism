# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **Distributed Caching Layer for System Packages** - High-Performance Package-Aware Transparent Proxy

[中文版本](README_zh.md)

## Vision

pacPrism aims to be a **distributed caching layer for system packages** that enhances existing package managers through transparent proxy mode.

### Target Architecture (Design Goals)

1. **Transparent Proxy** - Modify `sources.list` to intercept APT/Pacman requests
2. **Semantic Sharding** - Group related packages for locality and dependency completeness
3. **Hybrid Distribution** - Central gateway + decentralized P2P data layer
4. **Smart Routing** - Query DHT to find optimal nodes for content retrieval

### Current Reality

**This is early-stage prototype code.** Most features above are design goals, not working implementations.

## Implementation Status

### ✅ Working (Tested & Verified)
- **HTTP/1.1 Server** - Boost.Beast-based, starts on port 9001, returns basic responses
- **DHT Memory Index** - In-memory hash-based storage, basic CRUD operations work
- **Build System** - CMake Presets, cross-platform (Windows/Linux), vcpkg integration

### 🔄 Partial (Implemented but Incomplete)
- **HTTP Request Router** - Architecture exists, most functions are stubs
- **Custom Headers** - Can detect "Operation" header, no actual routing logic
- **Response Builder** - Basic string responses, variant-based handling framework

### 📋 Design (Code Not Written)
- **P2P Protocol** - 0% implementation
- **Real Distributed DHT** - Current DHT is single-process in-memory only
- **Semantic Sharding** - Data structure defined, no algorithm
- **Package-Aware Routing** - No actual APT request handling
- **Cache Fallback** - No official source integration

## Technology Stack

- **Core System**: **C++23** - High performance, low latency network processing
- **Network Library**: **Boost.Beast 1.89.0** - HTTP/1.1 asynchronous server
- **Build System**: **CMake 3.14+** - Cross-platform modular build
- **Dependency Management**: **vcpkg** - Automated dependency installation

## Quick Start

### Requirements
- **C++23 compatible compiler** (GCC 13+, Clang 14+, MSVC 19.36+)
- **CMake 3.19+** (for Presets support)
- **Visual Studio Build Tools** (Windows only, MSVC 19.36+)
- **vcpkg** package manager (local installation required)

### Setting up vcpkg

**First-time vcpkg setup** (if you don't have vcpkg installed):
```bash
# Clone vcpkg to a location of your choice (NOT inside this project)
git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg  # or ~/vcpkg on Linux/macOS
cd C:/vcpkg  # or cd ~/vcpkg
.\bootstrap-vcpkg.bat  # on Windows
# ./bootstrap-vcpkg.sh  # on Linux/macOS

# Set VCPKG_ROOT environment variable
# Windows (PowerShell):
$env:VCPKG_ROOT="C:/vcpkg"
# To make it permanent, add to System Environment Variables

# Linux/macOS (bash/zsh):
export VCPKG_ROOT=~/vcpkg
# Add to ~/.bashrc or ~/.zshrc for persistence
```

### Build & Run (Recommended)

**Using CMake Presets** (Cross-platform):
```bash
# Clone repository (no --recurse-submodules needed)
git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism

# Make sure VCPKG_ROOT environment variable is set
# Windows PowerShell: echo $env:VCPKG_ROOT
# Linux/macOS: echo $VCPKG_ROOT

# Configure and build (Debug)
cmake --preset debug
cmake --build --preset debug

# Or configure and build (Release)
cmake --preset release
cmake --build --preset release

# Run the application
./build/bin/pacprism
```

### Build & Run (Legacy)

**Windows (PowerShell):**
```powershell
# Make sure VCPKG_ROOT is set first
$env:VCPKG_ROOT="C:/vcpkg"  # Adjust path to your vcpkg location

git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS (Bash):**
```bash
# Make sure VCPKG_ROOT is set first
export VCPKG_ROOT=~/vcpkg  # Adjust path to your vcpkg location

git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism
chmod +x scripts/build.sh
./scripts/build.sh
./build/bin/pacprism
```

## Documentation

- [🗺️ Development Roadmap](docs/ROADMAP.md) - Three-phase evolution strategy: from campus to public P2P
- [📋 Current Development Status](docs/CURRENT_STATUS.md) - Detailed implementation progress and feature verification
- [🏗️ Project Architecture](docs/PROJECT_STRUCTURE.md) - Complete architecture design documentation
- [📝 Development Log](devlog_zh/README_DEVLOG.md) - Daily development records

## License

GPLv3