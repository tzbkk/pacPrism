#include "common.hpp"

// Forward declarations for test suite runners
void run_validator_tests();
void run_dht_tests();
void run_parser_tests();
void run_banner_tests();
void run_io_tests();
void run_transmission_tests();
void run_router_tests();

int main(int argc, char* argv[]) {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "   pacPrism Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    // Run all test suites
    run_validator_tests();
    run_dht_tests();
    run_parser_tests();
    run_banner_tests();
    run_io_tests();
    run_transmission_tests();
    run_router_tests();

    // Print summary
    test::print_summary();

    // Return exit code based on test results
    return (test::failed_tests == 0) ? 0 : 1;
}
