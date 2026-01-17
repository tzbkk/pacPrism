# pacPrism Test Suite

This directory contains the complete test suite for pacPrism modules.

## Directory Structure

```
test/
├── common.hpp                    # Test framework and assertion macros
├── main.cpp                      # Test suite entry point
├── CMakeLists.txt                # Test build configuration
├── README.md                     # This file
├── node/
│   ├── validator/
│   │   └── test_validator.cpp    # Validator and SHA256 tests
│   └── dht/
│       └── test_dht.cpp          # DHT operation tests
├── console/
│   ├── parser/
│   │   └── test_parser.cpp       # Command-line parser tests
│   ├── banner/
│   │   └── test_banner.cpp       # Banner display tests
│   └── io/
│       └── test_io.cpp           # I/O configuration tests
└── network/
    ├── transmission/
    │   └── test_transmission.cpp # HTTP server tests
    └── router/
        └── test_router.cpp       # HTTP router tests
```

## Building the Tests

### Option 1: Enable tests during CMake configuration

```bash
cmake -DBUILD_TESTING=ON -B build
cmake --build build
```

### Option 2: Build existing CMake configuration with testing enabled

```bash
cmake -DBUILD_TESTING=ON -B build
cmake --build build --target pacprism_tests
```

## Running the Tests

### Run all tests

```bash
./build/bin/pacprism_tests
```

Or use the custom target:

```bash
cmake --build build --target run_tests
```

### Run using CTest

```bash
cd build
ctest
```

## Test Coverage

### Validator Tests (test/node/validator/test_validator.cpp)
- SHA256 hash calculation
  - Empty file
  - Known content verification
  - Large files
  - Non-existent files
- SHA256 verification
  - Matching hashes
  - Non-matching hashes
  - Case-insensitive comparison
- Request validation
  - Plain client requests
  - Node requests with headers
  - Invalid partial headers

### DHT Tests (test/node/dht/test_dht.cpp)
- DHT initialization
- Entry verification (existent and non-existent)
- Entry storage and retrieval
- Shard-based node queries
- Duplicate node handling
- Multiple shards per node
- Expired entry cleanup
- Metadata storage
- Liveness tracking

### Parser Tests (test/console/parser/test_parser.cpp)
- Parser initialization
- Default argument parsing
- Custom port specification
- Help flag handling
- Invalid port detection
- Out-of-range port detection
- Config path resolution

### Banner Tests (test/console/banner/test_banner.cpp)
- Banner initialization
- Banner display output

### IO Tests (test/console/io/test_io.cpp)
- IO initialization
- Config loading with default paths

### Transmission Tests (test/network/transmission/test_transmission.cpp)
- Server creation
- Server start and stop operations

### Router Tests (test/network/router/test_router.cpp)
- Router initialization with DHT
- Operation routing (store operation)
- Plain client request handling

## Test Framework

The test suite uses a simple custom framework defined in `common.hpp`:

- **Assertion Macros**: `ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_NE`, `ASSERT_STREQ`, `ASSERT_STRNE`
- **Test Suites**: Organized by module using `test::TestSuite`
- **Colored Output**: ANSI color codes for pass/fail status
- **Summary Statistics**: Total tests passed/failed

## Adding New Tests

To add a new test case:

1. Create a test function that returns `bool`:
```cpp
bool test_my_feature() {
    // Setup
    MyClass obj;

    // Test
    ASSERT_TRUE(obj.some_condition());

    // Cleanup
    return true;
}
```

2. Add the test to the appropriate test suite:
```cpp
void run_my_module_tests() {
    test::TestSuite suite("My Module Tests");
    suite.add_test("My Feature", test_my_feature);
    suite.run();
}
```

3. Call the test runner from `main.cpp`:
```cpp
void run_my_module_tests();  // Forward declaration

int main() {
    // ...
    run_my_module_tests();
    // ...
}
```

## Continuous Integration

The test suite is designed to be integrated with CI/CD pipelines. Use the following exit codes:

- `0`: All tests passed
- `1`: One or more tests failed

Example for CI scripts:

```bash
#!/bin/bash
cmake -DBUILD_TESTING=ON -B build
cmake --build build --target pacprism_tests
./build/bin/pacprism_tests
exit $?  # Return test result as exit code
```

## Notes

- Tests create temporary files in the current working directory
- Network tests may use alternative ports to avoid conflicts
- Some tests are basic placeholders pending full implementation
- SHA256 tests use known hash values for verification
