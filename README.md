# pacPrism

> A semi-decentralized package distribution system for Debian GNU/Linux

## Description

pacPrism is a modern package distribution system that combines the reliability of centralized repositories with the resilience of peer-to-peer networks. Built with C++23 and leveraging Distributed Hash Table (DHT) technology, pacPrism provides enhanced package delivery while maintaining compatibility with existing Debian package management tools.

## Features

- **Semi-decentralized Architecture**: Uses DHT for resilient package distribution
- **Fast Package Delivery**: Download from multiple sources simultaneously
- **Enhanced Reliability**: Continue operations when central repositories are unavailable
- **Backward Compatible**: Works with existing Debian package management tools
- **Modern C++23**: Built with modern C++ standards and CMake

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

## Usage

pacPrism is currently in active development. The core architecture is being implemented with plans to integrate with existing Debian package management workflows.

## Project Structure

```
pacPrism/
├── CMakeLists.txt          # Root CMake configuration
├── src/                    # Main application source
├── lib/                    # Library components
├── include/                # Header files
└── docs/                   # Documentation and GitHub Pages
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

### Architecture

- **Main Application** (`src/main.cpp`): Entry point for the pacPrism executable
- **Network DHT Library** (`lib/network/dht/`): Distributed Hash Table implementation
- **Shared Library** (`network_dht`): Modular library for networking components

## License

GPLv3