# pacPrism

![pacPrism Banner](assets/prism-banner.png)

> **Distributed Caching Layer for System Packages** - High-Performance Package-Aware Transparent Proxy

[中文版本](README_zh.md)

## Core Value Proposition

pacPrism is a **distributed caching layer for system packages** that enhances the performance and reliability of existing package managers through transparent proxy mode. It acts as a local interceptor, seamlessly integrating with tools like APT/Pacman without requiring any additional client software.

### Technical Features

1. **Package-Aware Transparent Proxy**
   - Enable by simply modifying `sources.list`, completely transparent to package managers
   - Zero-configuration startup, maintaining full compatibility with existing workflows
   - Automatic identification of package types, dependencies, and version information

2. **Semantic Sharding Mechanism**
   - Group related packages into logical units (Shards)
   - Ensure dependency completeness and spatial locality
   - Reduce network round trips for cross-node dependency resolution

3. **Hybrid Distribution Architecture**
   - **Centralized Access Layer (Gateway)**: Provides unified entry point, compatible with existing mirror protocols
   - **Decentralized Data Layer (P2P)**: Nodes share package caches, reducing central server load
   - Effectively mitigates node churn issues in P2P networks

4. **Latency-Hidden Pipelining**
   - Overlap computation and transmission processes
   - Predictive prefetching of popular packages and their dependencies
   - Parallel acquisition of multiple shards

## Architecture Design

### Core Components

#### 1. HTTP Transparent Proxy Layer
```
sources.list: http://pacprism.local:8080/debian
                ↓
           pacPrism Gateway
                ↓
        [P2P Query] or [Official Source Fallback]
```

#### 2. Semantic Sharding System
- **Shard ID**: `core-utils`, `web-server`, `development-tools`
- **Package Mapping**: Organize logically related packages into the same shard
- **Spatial Locality**: Packages in the same shard typically cached on the same node

#### 3. DHT Index Layer
- **Key**: `package_name` or `shard_id`
- **Value**: `node_ip:port` list, sorted by node health
- **Query Routing**: Intelligent selection of optimal nodes for content retrieval

#### 4. Cache Fallback Mechanism
```
Request → Local Cache → P2P Nodes → Official Mirror
       ← Hit        ← Miss     ← Final Fallback
```

## Implementation Status

### ✅ Completed
- **HTTP Transparent Proxy Framework**: HTTP/1.1 server based on Boost.Beast
- **DHT Memory Index**: Basic distributed hash table implementation (O(1) query performance)
- **Cross-Platform Build System**: CMake + vcpkg automatic dependency management

### 🔄 In Progress
- **Package-Aware Routing Logic**: Intelligent distribution based on package names in HTTP headers
- **Semantic Sharding Algorithm**: Dependency analysis and grouping strategy implementation

### 🔬 Research Phase
- **P2P Content Distribution Protocol**: Node-to-node file transfer and cache synchronization mechanisms
- **Cache Prefetching Strategy**: Intelligent prefetching algorithms based on usage patterns

## Technology Stack

- **Core System**: **C++23** - High performance, low latency network processing
- **Network Library**: **Boost.Beast 1.89.0** - HTTP/1.1 asynchronous server
- **Build System**: **CMake 3.14+** - Cross-platform modular build
- **Dependency Management**: **vcpkg** - Automated dependency installation

## Quick Start

### Requirements
- **C++23 compatible compiler** (GCC 13+, Clang 14+, MSVC 19.36+)
- **CMake 3.14+**
- **Visual Studio Build Tools** (Windows only)

### Build & Run

**Windows:**
```powershell
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism
.\scripts\build.ps1
.\build\bin\pacprism.exe
```

**Linux/macOS:**
```bash
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism
chmod +x scripts/build.sh
./scripts/build.sh
./build/bin/pacprism
```

## Documentation

- [📋 Current Development Status](docs/CURRENT_STATUS.md) - Detailed implementation progress and feature verification
- [🏗️ Project Architecture](docs/PROJECT_STRUCTURE.md) - Complete architecture design documentation
- [📝 Development Log](devlog_zh/README_DEVLOG.md) - Daily development records

## License

GPLv3