#include "../../common.hpp"
#include <console/io/io.hpp>
#include <sstream>

// Test: Config initialization
bool test_config_initialization() {
    Config config;
    ASSERT_TRUE(true);  // If we got here, initialization succeeded
    return true;
}

// Test: Config set and get
bool test_config_set_get() {
    Config config;
    config.set("test_key", "test_value");
    std::string value = config.get("test_key");
    ASSERT_STREQ("test_value", value);
    return true;
}

// Test: Config get with default
bool test_config_get_default() {
    Config config;
    std::string value = config.get("nonexistent_key", "default_value");
    ASSERT_STREQ("default_value", value);
    return true;
}

// Test: Config has key
bool test_config_has_key() {
    Config config;
    config.set("existing_key", "some_value");
    ASSERT_TRUE(config.has("existing_key"));
    ASSERT_FALSE(config.has("nonexistent_key"));
    return true;
}

// Run all IO tests
void run_io_tests() {
    test::TestSuite suite("Config Tests");

    suite.add_test("Config: Initialization", test_config_initialization);
    suite.add_test("Config: Set and get", test_config_set_get);
    suite.add_test("Config: Get with default", test_config_get_default);
    suite.add_test("Config: Has key", test_config_has_key);

    suite.run();
}
