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

### Building the Project (Recommended)

**Using CMake Presets** (Cross-platform):
```bash
# List available presets
cmake --list-presets

# Configure with debug preset
cmake --preset debug

# Build with debug preset
cmake --build --preset debug

# Or configure and build with release preset
cmake --preset release
cmake --build --preset release

# Run the application
./build/bin/pacprism
```

### Building the Project (Legacy)

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
cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake

# Build the entire project
cmake --build build

# Build specific targets
cmake --build build --target pacprism
cmake --build build --target node_dht

# Clean build
cmake --build build --target clean
```

### Automatic Dependency Management

The build system automatically handles dependency installation via vcpkg (submodule):
- **vcpkg is a Git submodule** at `vcpkg/` directory
- **Boost.Beast 1.89.0** library is automatically fetched and configured
- Works seamlessly across Windows and Linux platforms
- Maintains compatibility with existing dependency management approaches
- No manual dependency installation required for new developers
- **Windows Prerequisite**: Visual Studio Build Tools with C++ compiler must be installed

**Note**: When cloning the repository, use `git clone --recurse-submodules` to ensure vcpkg is checked out.

### Running the Application

```bash
# Run pacPrism
./build/bin/pacprism
```

## Codebase Structure

```
pacPrism/
├── CMakeLists.txt              # Root CMake configuration
├── CMakePresets.json           # CMake Presets for simplified builds
├── vcpkg.json                  # vcpkg dependency management
├── README.md                   # English project documentation
├── README_zh.md               # Chinese project documentation
├── CLAUDE.md                   # Claude Code project guidance (this file)
│
├── scripts/                    # Build and configuration scripts (legacy)
│   ├── build.ps1              # Windows PowerShell build script
│   └── build.sh               # Linux/macOS Bash build script
│
├── cmake/                      # CMake configuration modules
│   ├── VersionConfig.cmake     # Version management configuration
│   ├── LibraryConfig.cmake     # Library target configuration
│   ├── BuildConfig.cmake       # Build system configuration
│   └── version.hpp.in          # Version header file template
│
├── src/                        # Main application source code
│   ├── CMakeLists.txt          # Executable configuration
│   └── main.cpp                # Application entry point
│
├── lib/                        # Library components
│   ├── CMakeLists.txt          # Library configuration
│   ├── network/                # Network layer implementation
│   │   ├── CMakeLists.txt      # Network library configuration
│   │   ├── transmission/       # HTTP transport layer
│   │   │   ├── CMakeLists.txt  # Transport library configuration
│   │   │   ├── transmission.cpp # HTTP server implementation
│   │   │   └── transmission.hpp # HTTP transport interface
│   │   └── router/             # HTTP request routing layer
│   │       ├── CMakeLists.txt  # Router library configuration
│   │       ├── router.cpp      # HTTP router implementation
│   │       └── router.hpp      # HTTP router interface
│   └── node/                   # Node functionality
│       └── dht/                # Distributed hash table
│           ├── CMakeLists.txt  # DHT library configuration
│           ├── dht_operation.cpp # DHT core operations (9-dimension index system)
│           ├── dht_operation.hpp # DHT operation interface (Node ID-based architecture)
│           └── dht_types.hpp     # DHT data structures (modern C++23 design)
│
├── include/                    # Header files (.hpp for C++ language detection)
│   ├── network/                # Network related headers
│   │   ├── transmission/       # HTTP transport headers
│   │   │   └── transmission.hpp # HTTP transport interface declaration
│   │   └── router/             # HTTP router headers
│   │       └── router.hpp      # HTTP router interface declaration
│   └── node/                   # Node related headers
│       ├── dht/                # DHT headers
│       │   ├── dht_operation.hpp # DHT operation interface declaration
│       │   └── dht_types.hpp     # DHT data structure definition
│       └── sharding/           # Sharding related
│           └── sharding_types.hpp # Sharding data structures (shard struct)
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

### HTTP Transport Layer (`lib/network/transmission/`)

**ServerTrans** class provides HTTP/1.1 server functionality:
- `start_server()`: Start server listening on configurable port (default: 9001)
- `start_accept()`: Async connection acceptance
- `read_from_connection()`: HTTP request reading with stream buffer management
- `response_builder()`: Response content construction delegates to Router
- `response_sender()`: Async response transmission

**Key Features:**
- Custom "Operation" HTTP header detection for package-aware routing
- Version information integration into HTTP responses
- Memory-safe async I/O with RAII and smart pointers
- Stream buffer proper usage with `prepare()`/`commit()`
- Router integration for DHT operation support

### HTTP Request Router (`lib/network/router/`)

**Router** class provides HTTP request routing with DHT integration (added 2025-12-21):
- `Router(DHT_operation&)`: Dependency injection constructor
- `route_operation()`: Main routing logic for "Operation" header handling
- `operation_store()`: DHT storage operation handler
- `plain_response_router()`: Default response handler (implementation pending)
- `node_response_router()`: Node query routing (placeholder)

**Architecture Features:**
- Loose coupling with DHT through dependency injection
- Foundation for HTTP API layer over DHT functionality
- Separation of HTTP routing concerns from DHT operations

### DHT Distributed Index (`lib/node/dht/`)

**DHT_operation** class provides Node ID-based distributed hash table with 9-dimensional indexing (completely refactored 2025-12-21):

**Core Interface Methods:**
- `verify_entry(node_id)`: Fast node existence validation (O(1))
- `store_entry(dht_entry)`: Timestamp-based intelligent storage with conflict resolution (O(k))
- `query_node_ids_by_shard_id(shard_id)`: O(1) shard-based node discovery
- `entry_builder(node_id)`: Dynamic entry construction (declared, not implemented)
- `clean_by_expiry_time()`: Automated expired node removal (O(n))
- `clean_by_liveness()`: Health-based node pruning (declared, not implemented)
- `remove_entry(node_id)`: Private method for complete node removal

**9-Dimensional Index System:**
- IP↔ID bidirectional mappings for flexible node identification
- Dual timestamp indexing (generation + expiry) for lifecycle management
- Shard indexing for efficient package distribution queries
- Metadata and liveness tracking for health monitoring

**Data Structures (2025-12-21 architecture):**
- `dht_entry`: Node information with `node_id`, IP, `std::set<shard>`, timestamps, metadata
- `shard`: Shard information with ID and package list (renamed from `sharding`)
- Complete modern C++23 design with RAII and exception safety

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

### Current Implementation Priority (Updated 2025-12-21)
1. **Complete Router Implementation** - Fix return statements and add missing functionality
2. **DHT Operation HTTP APIs** - Implement complete HTTP interface for all DHT operations
3. **Package-Aware Logic** - Intelligent distribution based on package names and shard analysis
4. **JSON Request/Response Processing** - Support structured data exchange
5. **Error Handling and Status Codes** - Comprehensive HTTP error management

### Testing and Verification
- HTTP server functionality: `curl http://localhost:9001/`
- Custom headers: `curl -H "Operation: store" http://localhost:9001/`
- DHT operations verification through unit tests
- Cross-platform build validation on Windows and Linux

## Core Implementation Files

### Main Application (`src/main.cpp`)
Application entry point that coordinates system initialization:
- DHT_operation instance creation
- Router dependency injection setup
- HTTP server startup with Router integration
- Version information display

### DHT Implementation (`lib/node/dht/`)
**dht_operation.cpp**: Complete implementation of 9-dimensional DHT index system
- Node ID-based architecture with IP↔ID bidirectional mapping
- Timestamp-based lifecycle management with dual indexing
- Shard-based package distribution with O(1) queries
- Metadata and health tracking for distributed operations

**Key Features:**
- Modern C++23 design with complete RAII
- Exception-safe operations with proper error handling
- O(1) core operations with multi-dimensional indexing
- Memory-efficient data structures with minimal overhead

### HTTP Router (`lib/network/router/`)
**router.cpp**: HTTP request routing with DHT integration
- Dependency injection pattern for loose coupling
- "Operation" header based routing logic
- Foundation for RESTful API over DHT operations
- Extensible routing framework for future protocols

### HTTP Transport (`lib/network/transmission/`)
**transmission.cpp**: High-performance HTTP/1.1 server
- Boost.Beast-based async I/O implementation
- Stream buffer management for memory safety
- Router integration for request processing
- Configurable binding and port management

## Architecture Highlights

### Node ID-Based Design
The DHT system uses Node IDs as primary identifiers, providing flexibility for:
- IPv4/IPv6/hostname node identification
- Future P2P network integration
- Dynamic node management and migration
- Abstract node representation for distributed computing

### Multi-Dimensional Indexing
Nine separate index structures provide optimal performance for different query patterns:
- **Identity**: IP↔ID mappings for flexible node lookup
- **Temporal**: Dual timestamp indexing for lifecycle management
- **Spatial**: Shard indexing for package distribution
- **Health**: Liveness and metadata tracking for system monitoring

### Dependency Injection Architecture
Router class accepts DHT_operation reference, enabling:
- Loose coupling between HTTP and DHT layers
- Easy testing and mocking capabilities
- Flexible DHT implementation swapping
- Clean separation of concerns

This architecture provides a solid foundation for distributed package management with high performance, scalability, and maintainability.