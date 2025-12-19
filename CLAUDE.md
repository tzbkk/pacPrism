# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

pacPrism is a **distributed caching layer for system packages** that enhances existing package managers through transparent proxy mode. It acts as a high-performance package-aware interceptor that seamlessly integrates with APT/Pacman tools.

### Core Technical Value Proposition

1. **Package-Aware Transparent Proxy**: Zero-configuration startup by modifying `sources.list`
2. **Semantic Sharding Mechanism**: Groups related packages for dependency completeness and spatial locality
3. **Hybrid Distribution Architecture**: Centralized access layer + decentralized P2P data layer
4. **Latency-Hidden Pipelining**: Overlapped computation and transmission with predictive prefetching

### Current Implementation Status

- **HTTP Transparent Proxy**: 85% complete - Boost.Beast HTTP/1.1 server with custom "Operation" header support
- **DHT Memory Index**: 90% complete - O(1) lookup performance with unordered_map optimization
- **P2P Communication Protocol**: 0% complete - Research phase
- **Semantic Sharding**: Algorithm design phase

For detailed implementation status, see [docs/CURRENT_STATUS.md](docs/CURRENT_STATUS.md).

## Development Commands

### Building the Project

**Windows (PowerShell):**
```powershell
# Run the automated build script
.\scripts\build.ps1
```

**Linux/macOS (Bash):**
```bash
# Run the automated build script
chmod +x scripts/build.sh
./scripts/build.sh
```

**Manual Build (Cross-platform):**
```bash
# Configure the project (automatically handles dependencies)
cmake -B build

# Build the entire project
cmake --build build

# Build specific targets
cmake --build build --target pacprism
cmake --build build --target node_dht

# Clean build
cmake --build build --target clean
```

### Automatic Dependency Management

The build system automatically handles dependency installation via vcpkg:
- **Boost.Beast 1.89.0** library is automatically fetched and configured
- Works seamlessly across Windows and Linux platforms
- Maintains compatibility with existing dependency management approaches
- No manual dependency installation required for new developers
- **Windows Prerequisite**: Visual Studio Build Tools with C++ compiler must be installed

### Running the Application

```bash
# Run pacPrism
./build/bin/pacprism
```

## Codebase Structure

```
pacPrism/
├── CMakeLists.txt              # Root CMake configuration
├── vcpkg.json                  # vcpkg dependency management
├── README.md                   # English project documentation
├── README_zh.md               # Chinese project documentation
├── CLAUDE.md                   # Claude Code project guidance (this file)
│
├── scripts/                    # Build and configuration scripts
│   ├── build.ps1              # Windows PowerShell build script
│   └── build.sh               # Linux/macOS Bash build script
│
├── cmake/                      # CMake configuration modules
│   ├── VersionConfig.cmake     # Version management configuration
│   ├── LibraryConfig.cmake     # Library target configuration
│   ├── BuildConfig.cmake       # Build system configuration
│   └── version.h.in            # Version header file template
│
├── src/                        # Main application source code
│   ├── CMakeLists.txt          # Executable configuration
│   └── main.cpp                # Application entry point
│
├── lib/                        # Library components
│   ├── CMakeLists.txt          # Library configuration
│   ├── network/                # Network layer implementation
│   │   ├── CMakeLists.txt      # Network library configuration
│   │   └── transmission/       # HTTP transport layer
│   │       ├── CMakeLists.txt  # Transport library configuration
│   │       ├── transmission.cpp # HTTP server implementation
│   │       └── transmission.h  # HTTP transport interface
│   └── node/                   # Node functionality
│       └── dht/                # Distributed hash table
│           ├── CMakeLists.txt  # DHT library configuration
│           ├── dht_operation.cpp # DHT core operations
│           ├── dht_operation.h  # DHT operation interface
│           └── dht_types.h      # DHT data structures
│
├── include/                    # Header files
│   ├── network/                # Network related headers
│   │   └── transmission/       # HTTP transport headers
│   │       └── transmission.h  # HTTP transport interface declaration
│   └── node/                   # Node related headers
│       ├── dht/                # DHT headers
│       │   ├── dht_operation.h  # DHT operation interface declaration
│       │   └── dht_types.h      # DHT data structure definition
│       └── sharding/           # Sharding related
│           └── sharding_types.h # Sharding data structures
│
├── docs/                       # Project documentation
│   ├── PROJECT_STRUCTURE.md    # Project architecture documentation
│   ├── CURRENT_STATUS.md       # Current development status
│   └── dht_operation.md        # DHT operation documentation
│
└── devlog_zh/                  # Chinese development logs
    └── README_DEVLOG.md        # Development log index
```

## Key Components

### HTTP Transparent Proxy (`lib/network/transmission/`)

**ServerTrans** class provides HTTP/1.1 server functionality:
- `start_server()`: Start server listening on port 8080
- `start_accept()`: Async connection acceptance
- `read_from_connection()`: HTTP request reading with stream buffer management
- `response_builder()`: Response content construction with "Operation" header support
- `response_sender()`: Async response transmission

**Key Features:**
- Custom "Operation" HTTP header detection for package-aware routing
- Version information integration into HTTP responses
- Memory-safe async I/O with RAII and smart pointers
- Stream buffer proper usage with `prepare()`/`commit()`

### DHT Memory Index (`lib/node/dht/`)

**dht_operation** class provides O(1) DHT functionality (optimized 2025-12-11):
- `store_entry()`: Store node entries with TTL management (O(1))
- `query_entry()`: Query by node IP, returns `std::optional<dht_entry>` (O(1))
- `clean_by_ttl()`: Automatic cleanup of expired entries
- `remove_entry()`: Private method for entry removal (accessible only by TTL cleanup)

**Data Structures (2025-12-11 optimized):**
- `dht_entry`: Node information with `unordered_map<std::string, sharding>` for O(1) shard lookup
- `sharding`: Shard information with ID and package list
- Uses `std::unordered_map` throughout for O(1) performance

## Technology Stack

- **Core System**: C++23 - High performance, low latency network processing
- **Network Library**: Boost.Beast 1.89.0 - HTTP/1.1 asynchronous server with Boost.Asio
- **Build System**: CMake 3.14+ - Modern target-based configuration with vcpkg integration
- **Dependency Management**: vcpkg - Automated cross-platform dependency installation
- **Third-party Dependencies**: Boost.Beast (includes Boost.Asio)

## Development Guidelines

### Code Quality Standards
- Use modern C++23 features (std::optional, smart pointers, RAII)
- Ensure memory safety with proper async callback lifecycle management
- Follow async I/O best practices with `shared_from_this()`
- Use unordered_map for O(1) lookup performance where applicable

### Current Implementation Priority
1. **Router Class Implementation** - Handle "Operation" HTTP header routing
2. **Package-Aware Logic** - Intelligent distribution based on package names
3. **DHT Integration** - Connect HTTP layer with DHT operations
4. **HTTP API Design** - RESTful interfaces for DHT operations

### Testing and Verification
- HTTP server functionality: `curl http://localhost:8080/`
- Custom headers: `curl -H "Operation: test" http://localhost:8080/`
- DHT operations verified through unit tests
- Cross-platform build verification on Windows and Linux