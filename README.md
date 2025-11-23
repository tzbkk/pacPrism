# pacPrism

![pacPrism Logo](assets/prism.svg)

> A semi-decentralized package distribution system for Debian GNU/Linux

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

- **Core System** (gateways, node logic, DHT/Gossip protocols): **C++**
  → Pursuing high performance, low latency, fine-grained memory control
- **Toolchain & Auxiliary Services** (monitoring, configuration management, test scripts): **Go**
  → Rapid development, concurrency-friendly, simple deployment

## Installation

### Prerequisites
- CMake 3.14 or higher
- C++23 compatible compiler

### Build Instructions

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

```
pacPrism/
├── CMakeLists.txt          # Root CMake configuration
├── src/                    # Main application source
├── lib/                    # Library components
├── include/                # Header files
└── docs/                   # Documentation and GitHub Pages
```

## Future Roadmap

- Support for additional distributions (Ubuntu, Arch, etc.)
- Content prefetching and intelligent caching strategies
- Visual topology and contribution leaderboards to incentivize node participation
- Exploration of interoperability with IPFS or BitTorrent protocols

## License

GPLv3