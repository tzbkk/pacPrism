#include "../../common.hpp"
#include <network/router/router.hpp>
#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <console/io/io.hpp>

// Test: Router initialization with all dependencies
bool test_router_initialization() {
    DHT_operation dht;
    Validator validator;
    Config config;

    // Create a minimal cache for testing
    FileCache cache(config, "./test_cache", "test.upstream.com");

    Router router(dht, validator, cache);
    ASSERT_TRUE(true);  // If we got here, initialization succeeded
    return true;
}

// Test: Router handles plain client request
bool test_router_plain_client() {
    DHT_operation dht;
    Validator validator;
    Config config;
    FileCache cache(config, "./test_cache", "test.upstream.com");

    Router router(dht, validator, cache);

    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/test");

    auto response = router.global_router(request);

    // Response should be valid (not null)
    ASSERT_TRUE(response.index() != 0);
    return true;
}

// Run all router tests
void run_router_tests() {
    test::TestSuite suite("Router Tests");

    suite.add_test("Router: Initialization", test_router_initialization);
    suite.add_test("Router: Plain client", test_router_plain_client);

    suite.run();
}
