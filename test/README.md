# pacPrism Integration Tests

This directory contains automated integration tests for pacPrism's production-ready features.

## Test Coverage

These tests verify the **~90% completed Phase 1 functionality**:

### HTTP Transparent Proxy Tests
1. **Basic Request** - Server responds with HTTP 200 and pacPrism banner
2. **File Download** - Real HTTP proxy (NOT HTTP 307 redirect)
3. **Range Request** - HTTP 206 Partial Content support
4. **Conditional Request** - HTTP 304 Not Modified support
5. **Query Parameter Path** - Alternative path format support

### DHT HTTP API Tests
6. **Store Entry** - POST /api/dht/store with JSON payload
7. **Verify Entry** - GET /api/dht/verify/{node_id}
8. **Query by Shard** - GET /api/dht/query?shard_id={id}
9. **Clean Expiry** - POST /api/dht/clean/expiry

## Prerequisites

1. **Build pacPrism**:
   ```bash
   cmake --preset release
   cmake --build --preset release
   ```

2. **Start the server** (in a separate terminal):
   ```bash
   # Windows PowerShell
   .\build\bin\pacprism.exe

   # Linux/macOS
   ./build/bin/pacprism
   ```

3. **Ensure curl is available**:
   - Windows: Use `curl.exe` (comes with Windows 10+)
   - Linux/macOS: Standard `curl` command

## Running Tests

### Windows (PowerShell)

```powershell
cd test
.\test_pacprism.ps1
```

### Linux/macOS (Bash)

```bash
cd test
chmod +x test_pacprism.sh
./test_pacprism.sh
```

## Expected Output

All tests should pass with output similar to:

```
========== pacPrism Integration Test Suite ==========

Server: http://localhost:9001

========== Checking Server Status ==========

✓ PASS: Server is running on http://localhost:9001

========== Running Tests ==========

[Test 1] Basic Request - GET /
✓ PASS: Basic request returned HTTP 200 with pacPrism banner

[Test 2] File Download - Verify Real Proxy (Not HTTP 307)
✓ PASS: Real HTTP proxy working (HTTP 200, not redirect)

[Test 3] Range Request - HTTP 206 Partial Content
✓ PASS: Range request returned HTTP 206 with Content-Range header

[Test 4] Conditional Request - If-Modified-Since
✓ PASS: Conditional request returned HTTP 304 Not Modified

...

========== Test Summary ==========

Total Tests:  9
Passed:       9
Failed:       0

All tests passed!
```

## Troubleshooting

### Server not responding

**Error**: `Server is not responding on http://localhost:9001`

**Solution**: Start the pacprism server first:
```bash
./build/bin/pacprism
```

### curl not found (Windows)

**Error**: `curl.exe : The term 'curl.exe' is not recognized`

**Solution**: Use Git Bash or WSL, or ensure you're using Windows 10+ with curl included.

### Connection refused

**Error**: `Failed to connect to localhost port 9001`

**Solution**:
1. Check if pacprism is running
2. Check firewall settings
3. Verify port 9001 is not in use by another application

### HTTP 307 redirects detected

**Error**: `Got HTTP 307 redirect instead of real proxy`

**Solution**: This indicates a bug in the proxy implementation. Check:
- FileCache integration in router.cpp
- Upstream fetching logic in io.cpp
- Verify FileCache::get_or_fetch() is called

## Test Logic Details

### Test 2: Real Proxy Verification
- Makes request to `/debian/README`
- Expects HTTP 200 (not HTTP 307)
- Verifies no `Location:` header is present
- Confirms proxy fetches from upstream and returns directly

### Test 3: Range Request
- Sends `Range: bytes=0-1023` header
- Expects HTTP 206 Partial Content
- Verifies `Content-Range:` header exists
- Tests APT resume functionality

### Test 4: Conditional Request
- Makes initial request to get `Last-Modified` header
- Makes second request with `If-Modified-Since` header
- Expects HTTP 304 Not Modified
- Tests bandwidth-saving functionality

### DHT API Tests (5-9)
- Uses custom headers: `pacPrism_node_id`, `pacPrism_node_signature`
- Tests JSON request/response
- Verifies all 5 DHT endpoints work correctly

## Next Steps After Testing

If all tests pass:
1. ✓ Core Phase 1 functionality is working
2. → Add logging system (spdlog or similar)
3. → Add metrics collection (hit rate, bandwidth savings)
4. → Write unit tests for individual components
5. → Production hardening (security, error handling)

If some tests fail:
1. Check the specific test failure message
2. Review relevant source code (router.cpp, io.cpp, validator.cpp)
3. Add debug output to identify root cause
4. Fix bug and retest

## Contributing

To add new tests:

1. Add test function to both `test_pacprism.sh` and `test_pacprism.ps1`
2. Call the test function in `main()`
3. Update this README with test description
4. Ensure tests work on both Windows and Linux/macOS

## Test Maintenance

These tests should be updated:
- When adding new HTTP endpoints
- When modifying existing HTTP behavior
- When changing DHT API contract
- When project advances to Phase 2 (distributed features)
