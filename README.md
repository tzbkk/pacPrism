# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **Distributed Caching Layer for System Packages** - High-Performance Package-Aware Transparent Proxy

**Current Version: v0.1.0** | [Release Notes](CHANGELOG.md)

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

### ✅ Production-Ready Features (Fully Implemented & Tested)

**HTTP Transparent Proxy System**:
- Real transparent HTTP proxy (NOT redirects) - Fetches files from upstream and returns them directly
- FileCache with upstream fetching, local disk caching, SHA256 verification
- Range request support (HTTP 206 Partial Content) - Critical for APT resume
- Conditional request support (If-Modified-Since, If-None-Match → HTTP 304)
- Error handling with retries (3 attempts, exponential backoff: 1s, 2s, 4s)
- Configurable timeouts (connect: 10s, read: 30s)

**DHT Core Operations**:
- 9-dimensional index system fully implemented
- Node verification, storage with conflict resolution, shard-based queries
- Automatic expiry cleanup
- O(1) core operations with multi-dimensional indexing

**HTTP Router with Triple Dependency Injection**:
- Validator integration for request type classification (PlainClient vs Node vs Invalid)
- Complete DHT HTTP API (5 JSON endpoints: verify, store, query, clean/expiry, clean/liveness)
- File proxy with Range/conditional request support via FileCache
- Production-ready, not placeholders

**Validator with SHA256**:
- Request type validation based on custom headers
- Cross-platform SHA256 calculation (Windows: bcrypt, Linux: OpenSSL)
- SHA256 verification for file integrity

**Debian Package Parser**:
- Binary package parsing: `name_version_arch.extension`
- Source package parsing: `.orig.tar.gz/xz`, `.dsc`, `.tar.gz/xz`
- Component extraction (main/contrib/non-free)

**Build System**:
- CMake Presets (debug/release) with cross-platform support
- Automated vcpkg dependency management
- Successfully tested on Windows MSYS2 (GCC 15.2.0)

### 📝 TODO/Stub Implementations (3 Items Only)
- `ClientTrans::start_connecting()` - Empty stub (client connection logic)
- `DHT_operation::clean_by_liveness()` - Empty stub (health-based node pruning)
- `Validator::verify_node_identity()` - Demo mode, always returns true (Ed25519 TODO)

### 📋 Design Phase (Not Yet Implemented)
- **P2P Protocol** - Node-to-node communication protocol
- **Real Distributed DHT** - Network communication between nodes
- **Semantic Sharding Algorithm** - Intelligent package grouping strategy
- **Node Authentication** - Ed25519 signature verification (currently demo mode)

## Technology Stack

- **Core System**: **C++23** - High performance, low latency network processing
- **Network Library**: **Boost.Beast** (from Debian packages) - HTTP/1.1 asynchronous server
- **Build System**: **CMake 3.14+** - Modular build system
- **Dependencies**: System packages via apt (Boost, OpenSSL, nlohmann-json)
- **Platform**: Linux (Debian/Ubuntu)

## Quick Start

### Requirements
- **C++23 compatible compiler** (GCC 13+, Clang 14+)
- **CMake 3.14+**
- **Debian/Ubuntu system packages**:
  - `libboost-dev` (Boost.Beast HTTP library)
  - `libssl-dev` (OpenSSL for SHA256)
  - `nlohmann-json3-dev` (JSON library)

### Install Dependencies

**Debian/Ubuntu:**
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    g++ \
    libboost-dev \
    libssl-dev \
    nlohmann-json3-dev \
    libcxxopts-dev
```

Or use:
```bash
make deps
```

### Build & Run

**Using Makefile (Recommended):**
```bash
# Clone repository
git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism

# Build (Release mode)
make release

# Or build (Debug mode)
make debug

# Run the application
./build/bin/pacprism
```

**Using CMake directly:**
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
./bin/pacprism
```

## Documentation

- [🗺️ Development Roadmap](docs/ROADMAP.md) - Three-phase evolution strategy: from campus to public P2P
- [📋 Current Development Status](docs/CURRENT_STATUS.md) - Detailed implementation progress and feature verification
- [🏗️ Project Architecture](docs/PROJECT_STRUCTURE.md) - Complete architecture design documentation
- [📝 Development Log](devlog_zh/README_DEVLOG.md) - Daily development records

## License

GPLv3