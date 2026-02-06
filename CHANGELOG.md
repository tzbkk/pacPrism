# Changelog

All notable changes to pacPrism will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- **BREAKING**: Dropped Windows support, Linux-only (Debian/Ubuntu)
- **BREAKING**: Removed vcpkg dependency management, now using system packages
- Removed all Windows-specific code (bcrypt, localtime_s)
- Simplified build system: Makefile instead of CMake Presets and build scripts
- Dependencies now installed via `apt`: libboost-dev, libssl-dev, nlohmann-json3-dev, libcxxopts-dev
- Replaced git submodule cxxopts with system package (libcxxopts-dev)

### Removed
- `CMakePresets.json` - vcpkg toolchain configuration
- `vcpkg.json` - vcpkg dependency manifest
- `scripts/build.ps1` - Windows build script
- `scripts/build.sh` - Legacy Linux build script
- `cmake/PlatformConfig.cmake` - Windows-specific platform config
- `cxxopts` git submodule - now using system package
- Windows bcrypt SHA256 implementation (validator.cpp)
- Windows localtime_s code (io.cpp)
- `.github/workflows/cmake-multi-platform.yml` - old CI workflow

### Added
- `Makefile` - Simple build system for Linux with `deps` target
- `.github/workflows/build.yml` - Simplified Linux-only CI workflow

---

## [0.1.0] - 2026-01-17

## [0.1.0] - 2026-01-17

### Added
- HTTP/1.1 server using Boost.Beast (async I/O)
- In-memory DHT (Distributed Hash Table) with 9-dimensional indexing
  - Node ID-based storage and lookup
  - IP↔ID bidirectional mappings
  - Dual timestamp indexing (generation + expiry)
  - Shard-based package distribution queries
  - Metadata and health tracking
- HTTP request router with DHT integration
- HTTP Range request support (RFC 7233)
- HTTP conditional request support (If-Modified-Since, If-None-Match)
  - Returns HTTP 304 Not Modified with empty body for unmodified resources
- SHA256 checksum verification
- DHT HTTP APIs with JSON support:
  - POST /api/dht/store - Store DHT entry (returns HTTP 201)
  - GET /api/dht/verify/{node_id} - Verify node existence
  - GET /api/dht/query?shard_id={id} - Query nodes by shard
  - POST /api/dht/clean/expiry - Remove expired entries
  - POST /api/dht/clean/liveness - Remove unhealthy entries
- File caching system with upstream proxy
- Debian package path parser
- Comprehensive integration test suite (Bash and PowerShell)

### Fixed
- HTTP 304 conditional requests now correctly use `http::empty_body` instead of `http::file_body`
- HTTP 307 redirect issue documented (proxy returns redirect instead of actual content)
- OpenSSL linking for SHA256 support on Linux
- HTTP status code expectation for DHT store operation (HTTP 201, not 200)

### Technical Details
- **Build System**: CMake 3.14+ with Make
- **Dependencies**: Boost.Beast (Debian system packages)
- **Dependency Management**: apt (system packages)
- **Language**: C++23
- **Platform**: Linux (Debian/Ubuntu)

### Known Limitations
- Linux-only (Debian/Ubuntu)
- HTTP proxy returns HTTP 307 redirect instead of actual file content (does not work with APT)
- DHT is single-process in-memory only (not truly distributed)
- No P2P communication implementation
- File caching exists but upstream proxy not fully implemented

### Documentation
- Updated CLAUDE.md with pragmatic project status
- Fixed severely outdated documentation in README files
- Updated implementation status across all documentation

## [Unreleased]

---

## Version History

- **0.1.0** (2026-01-17) - Initial release with HTTP server, DHT, and basic proxy features
