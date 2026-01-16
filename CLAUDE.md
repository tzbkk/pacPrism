# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Developer Context

**Developer**: Freshman student at NWPU (Northwestern Polytechnical University)
**Project Nature**: Learning project + practical utility tool
**Development Pace**: Part-time development, no hard deadlines

**My Role**: Assistant. I share 50% responsibility if this project fails.

## Project Overview

pacPrism is a **distributed caching layer for system packages** that enhances existing package managers (APT/Pacman) through transparent proxy mode. The core goal is to make software package downloads faster and more bandwidth-efficient within campus networks.

### Target Architecture (Not Yet Implemented)

1. **Transparent Proxy** - Modify `sources.list` to intercept APT requests
2. **Semantic Sharding** - Group related packages for dependency completeness and spatial locality
3. **Hybrid Distribution** - Central gateway + decentralized P2P data layer
4. **Smart Routing** - Query DHT to find optimal nodes for content retrieval

### Current Reality

**This is early-stage prototype code.** Most features above are design goals, not working implementations.

**What Works**:
- HTTP/1.1 server (Boost.Beast, port 9001, returns basic responses)
- DHT in-memory index (single-process hash table, basic CRUD operations)
- Build system (CMake Presets, cross-platform)

**What's Incomplete**:
- HTTP Router architecture exists but most functions are stubs
- Cannot handle real APT requests
- No true distributed functionality (single-process in-memory only)
- No file caching, P2P communication, or package-aware routing

For detailed status, see [docs/CURRENT_STATUS.md](docs/CURRENT_STATUS.md).

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
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS (Bash):**
```bash
chmod +x scripts/build.sh
./scripts/build.sh
./build/bin/pacprism
```

### Automatic Dependency Management

The build system automatically handles dependency installation via vcpkg (local installation):
- **Boost.Beast 1.89.0** - HTTP/1.1 asynchronous server (includes Boost.Asio)
- Requires VCPKG_ROOT environment variable to be set
- vcpkg should be installed separately (not as a submodule)

**Setting up vcpkg:**
```bash
# Install vcpkg to a location of your choice
git clone https://github.com/Microsoft/vcpkg.git C:/vcpkg  # or ~/vcpkg
cd C:/vcpkg
.\bootstrap-vcpkg.bat  # Windows: bootstrap-vcpkg.sh on Linux/macOS

# Set VCPKG_ROOT environment variable
# Windows PowerShell: $env:VCPKG_ROOT="C:/vcpkg"
# Linux/macOS: export VCPKG_ROOT=~/vcpkg
```

**Windows Prerequisite**: Install Visual Studio Build Tools with C++ compiler first

## Codebase Structure

```
pacPrism/
├── CMakeLists.txt              # Root CMake configuration
├── CMakePresets.json           # CMake Presets for simplified builds
├── vcpkg.json                  # vcpkg dependency management
├── README.md                   # English project documentation
├── README_zh.md               # Chinese project documentation
├── CLAUDE.md                   # This file
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
│   ├── network/
│   │   ├── transmission/       # HTTP transport layer
│   │   │   ├── transmission.cpp # HTTP server implementation
│   │   │   └── transmission.hpp # HTTP transport interface
│   │   └── router/             # HTTP request routing layer
│   │       ├── router.cpp      # HTTP router implementation
│   │       └── router.hpp      # HTTP router interface
│   └── node/                   # Node functionality
│       └── dht/                # Distributed hash table
│           ├── dht_operation.cpp # DHT core operations
│           ├── dht_operation.hpp # DHT operation interface
│           └── dht_types.hpp     # DHT data structures
│
├── include/                    # Header files (.hpp for C++ language detection)
│   ├── network/                # Network related headers
│   │   ├── transmission/
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
│   ├── ROADMAP.md              # Development roadmap
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

### Project Philosophy: Pragmatic Path to Production

**Goal**: Production-grade distributed package cache
**Current State**: Prototype phase - functionality incomplete, untested for production load

**Core Principles:**
1. **Shipping > Design** - Working features beat elegant architecture
2. **Test-Driven Honesty** - If it's not tested, it's not done
3. **Production Standard** - Edge cases, errors, performance matter
4. **No Buzzword Inflation** - "9-dimensional index" is implementation detail, not feature

**Devlog Standards:**
- What **actually works** today (tested with curl/integration tests)
- What **broke** today (not just "refactored")
- What's **blocking** production (honest assessment)
- Progress % = **tested functionality** / **required functionality**
- "Refactored" = changed working code, not "rewrote because first version was wrong"

**Reality Check:**
- HTTP server that returns "Hello" = 5% of production proxy
- In-memory hash maps = 0% of distributed system
- Design documents ≠ working system

### Current Implementation Priority (Updated 2026-01-17)
1. **Implement True Transparent Proxy** - Replace HTTP 307 redirects with actual proxy functionality
2. **File Caching System** - Cache upstream responses to local filesystem
3. **Complete Router Implementation** - Fix return statements and add missing functionality
4. **DHT Operation HTTP APIs** - Implement complete HTTP interface for all DHT operations
5. **Package-Aware Logic** - Intelligent distribution based on package names and shard analysis
6. **JSON Request/Response Processing** - Support structured data exchange
7. **Error Handling and Status Codes** - Comprehensive HTTP error management

### HTTP Proxy Implementation Status

**Current Behavior (NOT Production Ready)**:
- Router returns **HTTP 307 Temporary Redirect** to upstream server
- Example: `GET /debian/pool/main/...` → 307 → `http://debian.org/debian/pool/main/...`
- This works for browsers with `-L` flag but **fails for APT clients**
- APT does NOT follow redirects - it expects direct file content

**Required Implementation**:
```cpp
// Current (BROKEN for APT):
return redirect_builder("http://debian.org/" + path, version);

// Needed (WORKING for APT):
// 1. Make HTTP request to upstream
// 2. Stream response back to client
// 3. Cache to local disk for future requests
return proxy_response_builder(upstream_request, version);
```

**What APT Expects**:
```
GET /debian/pool/main/v/vim/vim_9.0.0.deb HTTP/1.1
Host: proxy.example.com:9001

Response (HTTP 200):
Content-Type: application/vnd.debian.binary-package
Content-Length: 1234567
[binary file data]
```

**What We Currently Return** (BROKEN):
```
HTTP/1.1 307 Temporary Redirect
Location: http://debian.org/debian/pool/main/v/vim/vim_9.0.0.deb
```

**Implementation Plan**:
1. **Phase 1**: HTTP client to fetch from upstream (Boost.Beast HTTP client)
2. **Phase 2**: Stream response back to APT client
3. **Phase 3**: Cache files to disk (std::filesystem, SHA256 verification)
4. **Phase 4**: Serve from cache if available
5. **Phase 5**: Handle Range requests and conditional requests (If-None-Match, If-Modified-Since)

### Testing and Verification

**Basic HTTP Server**:
```bash
curl http://localhost:9001/
# Returns: "Hello from pacPrism!" + version info
```

**Custom Headers**:
```bash
curl -H "Operation: store" http://localhost:9001/
# Returns: "Operation: store" + version info
# (Does NOT actually store anything yet)
```

**Debian Package Path (Current - Redirect)**:
```bash
curl -v http://localhost:9001/debian/pool/main/g/gnome-extra-icons/gnome-extra-icons_1.1-5.dsc
# Returns: HTTP 307 Redirect to http://debian.org/...
# This DOES NOT work for APT clients!
```

**Debian Package Path (Needed - Proxy)**:
```bash
# After implementing true proxy:
curl -v http://localhost:9001/debian/pool/main/v/vim/vim_9.0.0.deb
# Should return: HTTP 200 with file content (NOT redirect!)
```

**Windows PowerShell Note**:
- PowerShell's `curl` is `Invoke-WebRequest` alias
- Use `curl.exe` for real curl or test from Git Bash/WSL

## Learning Roadmap

### Knowledge Areas for Current Project Phase

#### 1. HTTP Protocol Basics (Current Need)
**Why it's needed**: Router needs to parse real APT requests
**What to learn**:
- HTTP request/response format
- HTTP header fields
- HTTP status codes
- How APT uses HTTP (Range requests, conditional requests)
- **Resources**: MDN Web Docs - HTTP, RFC 7231

#### 2. JSON Serialization (Immediate Need)
**Why it's needed**: DHT HTTP API needs structured data exchange
**What to learn**:
- JSON format basics
- C++ JSON libraries (nlohmann/json or similar)
- Serialization/deserialization
- **Resources**: nlohmann/json GitHub README, JSON RFC 8259

#### 3. File I/O and Caching (Medium-term Need)
**Why it's needed**: Need to persist package files to disk
**What to learn**:
- C++ file streams (fstream)
- Filesystem operations (std::filesystem)
- LRU cache algorithms
- Memory-mapped files (mmap)
- **Resources**: cppreference filesystem

#### 4. Advanced Network Programming (Medium-term Need)
**Why it's needed**: P2P communication
**What to learn**:
- TCP/UDP socket programming
- Async I/O models (already have basics through Boost.Asio)
- Network byte order
- **Resources**: Boost.Asio documentation, Beej's Guide to Network Programming

#### 5. Distributed Systems Concepts (Long-term Need)
**Why it's needed**: True distributed DHT
**What to learn**:
- Consistent hashing
- Kademlia DHT protocol
- Gossip protocol
- CAP theorem
- **Resources**: DDIA ("Designing Data-Intensive Applications")

#### 6. Security Basics (Long-term Need)
**Why it's needed**: Public deployment needs security mechanisms
**What to learn**:
- TLS/HTTPS basics
- GPG signature verification
- Basic authentication and authorization
- **Resources**: Let's Encrypt cryptography primers

### Learning Strategy Advice

**Freshman has limited time**, don't try to learn everything at once:
1. **Current phase**: Only learn HTTP protocol basics (just enough to get by)
2. **When problems arise**: Look up specific knowledge points as needed
3. **Practice first**: Learn while doing, don't do pure theory study
4. **Take notes**: Write key learnings into devlog

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

## Git Commit Strategy

**Current Strategy**: Don't commit until achieving the next major breakthrough

This means the working directory will stay "dirty" - this is normal. Priorities are:
1. Implement working features
2. Test and verify
3. Then consider committing

Don't stress about uncommitted changes in the working directory. This is a freshman learning project - there's no need to maintain a perfect Git history.
