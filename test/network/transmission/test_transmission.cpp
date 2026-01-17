#include "../../common.hpp"
#include <network/transmission/transmission.hpp>
#include <network/router/router.hpp>
#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <console/io/io.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

// Test: ServerTrans creation with all dependencies
bool test_transmission_creation() {
    boost::asio::io_context io_context;
    DHT_operation dht;
    Validator validator;
    Config config;
    FileCache cache(config, "./test_cache", "test.upstream.com");
    Router router(dht, validator, cache);

    auto server = ServerTrans::create(io_context, router);
    ASSERT_TRUE(server != nullptr);
    return true;
}

// Run all transmission tests
void run_transmission_tests() {
    test::TestSuite suite("Transmission Tests");

    suite.add_test("Transmission: Creation", test_transmission_creation);

    suite.run();
}
