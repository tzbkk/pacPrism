# pacPrism

![pacPrism Banner](assets/prism-banner.svg)

> A semi-decentralized package distribution system for Debian GNU/Linux

[中文版本](README_zh.md)

## Overview

pacPrism is a revolutionary package distribution system that addresses the fundamental limitations of traditional "center-mirror" software distribution models. By combining **access-layer centralization** with **data-layer decentralization**, pacPrism delivers enhanced reliability, reduced latency, and lower operational costs while maintaining complete compatibility with existing Debian package management tools.

### Core Philosophy

- **User Experience First**: Completely transparent to end users - simply modify `sources.list` to point to pacPrism gateway, no additional clients or plugins required
- **Aristotelian Virtue Ethics**: Implements a "virtue-driven" node evaluation mechanism based on reliability, contribution, and local reputation, avoiding blockchain-style global ledgers
- **Extensible Vision**: Designed to eventually support any binary artifact distribution (Docker images, npm packages, etc.), particularly benefiting network-constrained or bandwidth-expensive regions

## Architecture

### Hybrid Architecture Model

pacPrism employs a **"centralized access layer + decentralized data layer"** hybrid approach:

- **Access Layer (Centralized)**: **Gateway clusters** provide unified external services as the sole user entry point, ensuring compatibility and control
- **Data Layer (Decentralized)**: **P2P network** (DHT + Gossip) distributes `.deb` packages and metadata among server nodes, achieving load distribution and redundancy tolerance

### Core Components

#### 1. Gateway Cluster (The "Intelligent Brain")
- Receives HTTP/HTTPS requests from APT clients (simulating official repository behavior)
- Queries DHT to obtain available node lists for target files
- Performs intelligent scheduling based on node weight, latency, reputation
- Automatically falls back to official sources when P2P network is unresponsive
- Provides TLS termination, rate limiting, log auditing, and other operational capabilities

#### 2. Peer Nodes (The "Backbone Muscles")
- Store partial or complete Debian package replicas
- Participate in DHT routing and Gossip protocol propagation
- Respond to file requests from gateways or other nodes
- Report heartbeat and local status (for anti-fragility mechanisms)

#### 3. DHT & Gossip Network (The "Neural Network")
- **DHT (Distributed Hash Table)**: Efficient file location (Key = package name/hash, Value = node address list)
- **Gossip Protocol**: Propagates node join/exit, health status, local reputation updates
- **Dual DHT System**: Separate DHTs for lightweight nodes (low storage, high availability) and heavyweight nodes (high capacity, stable contribution)

#### 4. Official Sources (The "Trust Anchor")
- Serves as the ultimate fallback source, ensuring content authenticity and integrity
- Provides initial metadata (`Packages.gz`, `Release` files) for verification
- All content from P2P network must pass verification against official source signatures

## Key Mechanisms

### Security-First Design
- All transmitted content must pass signature verification from official sources
- Gateways do not cache unverified content, preventing contamination spread
- Node identities can be authenticated via certificates or pre-shared keys (PSK)

### Active Anti-Fragility
- **Heartbeat Mechanism**: Nodes regularly report status, gateways dynamically adjust routing weights
- **IPv6 Optimization**: Leverages IPv6 multicast for node discovery, reducing central registration dependency
- **Graceful Degradation**: Seamless fallback to official sources when P2P is unavailable, transparent to users

### Virtue-Driven Node Evaluation
- No global blockchain records; based on:
  - **Local Reputation** (historical behavior evaluations from neighbor nodes)
  - **Verifiable Contribution** (successful valid file deliveries count/byte count)
  - **Implicit Weight** (online duration, bandwidth stability, etc.)
- Weights used for DHT routing priority and gateway scheduling decisions

## Technology Stack

- **Core System** (gateways, node logic, DHT/Gossip protocols): **C++23**
  → High performance, low latency, fine-grained memory control
- **Build System**: **CMake 3.14+**
  → Modern CMake practices with modular configuration
- **Networking**: **Boost.Beast** (includes built-in Boost.Asio)
  → HTTP/1.1 server implementation with async I/O
- **Package Management**: **vcpkg**
  → Cross-platform dependency management
- **Third-party Dependencies**: **Boost.Beast 1.89.0**
  → HTTP and WebSocket library for C++ (includes Boost.Asio for async I/O)

## Documentation

- [Project Structure](docs/PROJECT_STRUCTURE.md) - Complete project architecture and file organization
- [Version System](docs/VERSION_SYSTEM.md) - Modular version management system
- [Current Status](docs/CURRENT_STATUS.md) - Development status and roadmap
- [Chinese Devlog](devlog_zh/README_DEVLOG.md) - Development progress log (Chinese)

## Installation

### Prerequisites
- **CMake 3.14+**
- **C++23 compatible compiler** (GCC 13+, Clang 14+, MSVC 19.36+)
- **Visual Studio Build Tools** (Windows only, required for vcpkg to configure C++ dependencies)
- **vcpkg** (for auto dependency management)
- **Git** (for version information)

### Build Instructions

**Windows (PowerShell):**
```powershell
# Clone the repository (including submodules)
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism

# Run the automated build script
.\scripts\build.ps1

# Run pacPrism
.\build\bin\pacprism.exe
```

**Linux/macOS (Bash):**
```bash
# Clone the repository (including submodules)
git clone --recurse-submodules https://github.com/tzbkk/pacPrism.git
cd pacPrism

# Run the automated build script
chmod +x scripts/build.sh
./scripts/build.sh

# Run pacPrism
./build/bin/pacprism
```

**Manual Build (Cross-platform):**
```bash
# Clone the repository
git clone https://github.com/tzbkk/pacPrism.git
cd pacPrism

# Configure the project
cmake -B build

# Build the project
cmake --build build

# Run pacPrism
./build/bin/pacprism
```

## Development

### Building Specific Targets

```bash
# Build only the main executable
cmake --build build --target pacprism

# Build only the network library
cmake --build build --target network_dht

# Clean build
cmake --build build --target clean
```

### Project Structure

For detailed project architecture and file organization, see [Project Structure Documentation](docs/PROJECT_STRUCTURE.md).

## Future Roadmap

- Support for additional distributions (Ubuntu, Arch, etc.)
- Content prefetching and intelligent caching strategies
- Visual topology and contribution leaderboards to incentivize node participation
- Exploration of interoperability with IPFS or BitTorrent protocols

## License

GPLv3