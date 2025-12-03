# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

pacPrism is a semi-decentralized package distribution system for Debian GNU/Linux. It combines centralized access with decentralized data distribution to improve reliability, reduce latency, and lower operational costs while maintaining compatibility with existing Debian package management tools.

### Core Architecture

The system follows a "centralized access layer + decentralized data layer" hybrid approach:

1. **Gateway Cluster** (Access Layer): Provides unified external services and acts as the sole user entry point
2. **Peer Nodes** (Data Layer): Store Debian package replicas and participate in P2P networks
3. **DHT & Gossip Network**: Enables efficient file location and node communication
4. **Official Sources**: Ultimate fallback ensuring content authenticity and integrity

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

The build system automatically handles dependency installation:
- **asio** library is automatically fetched and configured if not found locally
- Works seamlessly across Windows and Linux platforms
- Maintains compatibility with existing dependency management approaches
- No manual dependency installation required for new developers

### Running the Application

```bash
# Run pacPrism
./build/bin/pacprism
```

## Codebase Structure

```
pacPrism/
├── CMakeLists.txt          # Root CMake configuration
├── src/                    # Main application source
│   ├── CMakeLists.txt      # Executable configuration
│   └── main.cpp            # Application entry point
├── lib/                    # Library components
│   ├── CMakeLists.txt      # Library configuration
│   └── node/dht/           # DHT implementation
│       └── dht_operation.cpp
├── include/                # Header files
│   └── node/
│       ├── dht/
│       │   ├── dht_operation.h
│       │   └── dht_types.h
│       └── sharding/
│           └── sharding_types.h
└── docs/                   # Documentation and GitHub Pages
```

## Key Components

### DHT Operations (`lib/node/dht/dht_operation.cpp`)

Implements core Distributed Hash Table functionality:
- `store_entry()`: Stores DHT entries with TTL management
- `query_entry()`: Queries entries by node IP or sharding
- `remove_entry()`: Removes entries by node IP
- `clean_by_ttl()`: Cleans expired entries based on TTL

### Data Structures

- `dht_entry`: Represents a node entry with IP, sharding info, timestamp, and TTL
- `sharding`: Represents sharding information with ID and package list

## Technology Stack

- **Core System**: C++23 for high performance and fine-grained memory control
- **Build System**: CMake 3.14+
- **Project Management**: Modern CMake practices with target-based approach

## Current Development Status

As of the last development log (2025-11-23):
- ✅ DHT core operations implemented (store, query, remove, TTL cleanup)
- ✅ Build system fixed for cross-platform compatibility
- ✅ Project structure optimized with node/dht organization
- Next steps: Implement DHT network communication layer and gateway infrastructure