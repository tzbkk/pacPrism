#include "../../common.hpp"
#include <console/banner/banner.hpp>
#include <sstream>
#include <iostream>

// Test: Banner initialization
bool test_banner_initialization() {
    Banner banner;
    ASSERT_TRUE(true);  // If we got here, initialization succeeded
    return true;
}

// Test: Banner prints output
bool test_banner_print() {
    Banner banner;

    // Redirect cout to capture output
    std::streambuf* old = std::cout.rdbuf();
    std::stringstream ss;
    std::cout.rdbuf(ss.rdbuf());

    banner.print();

    // Restore cout
    std::cout.rdbuf(old);

    std::string output = ss.str();

    // Banner should contain some text
    ASSERT_TRUE(!output.empty());

    return true;
}

// Run all banner tests
void run_banner_tests() {
    test::TestSuite suite("Banner Tests");

    suite.add_test("Banner: Initialization", test_banner_initialization);
    suite.add_test("Banner: Print", test_banner_print);

    suite.run();
}
