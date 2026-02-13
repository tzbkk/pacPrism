# pacPrism - Agent Guidelines

> Version: v0.1.0 | Language: C++23 | Build: CMake 3.14+ + Make

## Build Commands

```bash
make deps              # Install system dependencies via apt
make release           # Build release mode (default)
make debug             # Build debug mode
make clean             # Remove build directory

# Direct CMake
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
./bin/pacprism
```

## Test Commands

**No automated test framework** - manual testing only:

```bash
# Run full integration test suite
cd test && bash test_pacprism.sh

# Run individual test manually
./build/bin/pacprism &                    # Start server in background
curl http://localhost:9001/               # Test 1: Basic request
curl http://localhost:9001/debian/README  # Test 2: File download
curl -H "Range: bytes=0-1023" http://localhost:9001/debian/README  # Test 3: Range request
curl -H "If-None-Match: <etag>" http://localhost:9001/debian/README  # Test 4: Conditional
curl http://localhost:9001/api/dht/verify/test_node  # Test 5: DHT verify
```

**No lint commands** - run `make release` to verify compilation.

## Code Style Guidelines

### File Structure & Headers
- Header guards: Use `#pragma once` (NOT `#ifndef` guards)
- Include order: Standard library → Boost → Project headers
- Separate implementation in `.cpp` from declarations in `.hpp`
- Headers go in `include/`, sources in `src/`

### Naming Conventions
- **Classes**: PascalCase (`ServerTrans`, `FileCache`, `Validator`)
- **Functions/Methods**: snake_case (`get_or_fetch`, `verify_entry`, `start_accept`)
- **Variables**: snake_case (`node_id`, `cache_path`, `http_code`)
- **Member variables**: `m_` prefix (`m_io_context`, `m_router`, `m_cache`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_RETRIES`, `BUFFER_SIZE`)
- **Types**: PascalCase for user-defined (`RequestType`, `RangeInfo`, `dht_entry`)
- **Namespaces**: lowercase (`beast`, `http`, `net`, `fs` for Boost.Beast aliases)

### Formatting
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style (opening brace on same line)
- **Line length**: Keep under 100-120 characters
- **Comments**: Minimal - do NOT add comments unless explicitly requested
- **Blank lines**: Single blank line between functions, two between sections

### Types & Pointers
- Use `std::shared_ptr<T>` for response objects
- Use `std::variant<T1, T2, ...>` for multiple response types
- Pass large objects by `const std::string&` reference
- Use references for dependency injection (`DHT_operation& dht`)
- Use `std::unique_ptr` for exclusive ownership (`std::unique_ptr<tcp::acceptor>`)
- Use `auto` with lambdas and iterators, explicit types otherwise

### Boost.Beast Patterns
```cpp
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// Response types
using router_response = std::variant<
    std::shared_ptr<http::response<http::string_body>>,
    std::shared_ptr<http::response<http::file_body>>,
    std::shared_ptr<http::response<http::empty_body>>
>;
```

### Error Handling
- Use `try-catch` for Boost.Beast operations and JSON parsing
- Print errors to `std::cerr` with descriptive messages
- Return appropriate HTTP status codes via `default_response_builder()`
- Check error codes: `if (ec) { std::cerr << "Error: " << ec.message(); return; }`
- Use `bool` return values for simple validation functions
- Throw `std::runtime_error` for recoverable errors with retry logic

### Common Patterns

**Factory Pattern with Private Constructor:**
```cpp
class ServerTrans {
public:
    static std::shared_ptr<ServerTrans> create(net::io_context& io, Router& router) {
        return std::shared_ptr<ServerTrans>(new ServerTrans(io, router));
    }
private:
    explicit ServerTrans(net::io_context& io, Router& router) : m_io_context(io), m_router(router) {}
};
```

**Dependency Injection:**
```cpp
class Router {
public:
    Router(DHT_operation& dht, Validator& validator, FileCache& cache);
};
```

**HTTP Routing:**
```cpp
auto response = m_router.global_router(request);
std::visit([](auto&& r) {
    http::async_write(*socket, *r, [](auto ec, auto) { /* handle */ });
}, response);
```

**File Operations:**
```cpp
beast::error_code ec;
response->body().open(cache_path.c_str(), beast::file_mode::read, ec);
if (ec) { std::cerr << "Failed to open: " << ec.message(); return nullptr; }
```

### Adding New Code
1. Add `.hpp` to `include/` directory
2. Add `.cpp` to `src/` directory
3. Update `src/CMakeLists.txt`:
   - `add_library(name SHARED source.cpp)`
   - `configure_library_target(name)`
   - `target_link_libraries(name PRIVATE dependencies)`
   - `get_version_info(name)`
4. Link in main executable or other libraries

### HTTP Routes
- Plain client: Direct paths (`/debian/...`) or query param (`/?target=/...`)
- Node API: `/api/dht/{operation}` format (verify, store, query, clean/expiry, clean/liveness)
- All responses: Set `"server"` header with version info
- Status codes: 200 OK, 201 Created, 206 Partial Content, 304 Not Modified, 4xx/5xx for errors

### Configuration
- Config file: `config/pacprism.conf` (key=value format)
- Access via `Config` class: `config.get_upstream()`, `config.get_cache_dir()`
- Defaults: upstream=`ftp.debian.org`, cache_dir=`./cache`

### Important Notes
- **No comments** in code unless explicitly requested
- **Linux-only** - removed Windows support
- **No automated tests** - manual testing with curl only
- **No linting** - just ensure compilation succeeds
- **No structured logging** - use `std::cout`/`std::cerr`
- Dependencies: Boost.Beast, OpenSSL, nlohmann-json, cxxopts (via apt)
