#include "../../common.hpp"
#include <console/parser/parser.hpp>

// Test: Parser initialization
bool test_parser_initialization() {
    Parser parser;
    ASSERT_TRUE(true);  // If we got here, initialization succeeded
    return true;
}

// Test: Parse default arguments (no arguments)
bool test_parser_default_args() {
    const char* argv[] = {"pacprism"};
    int argc = 1;

    Parser parser;
    bool parsed = parser.parse(argc, const_cast<char**>(argv));

    ASSERT_TRUE(parsed);
    ASSERT_EQ(9001, parser.get_port());  // Default port
    return true;
}

// Test: Parse with custom port
bool test_parser_custom_port() {
    const char* argv[] = {"pacprism", "--port", "8080"};
    int argc = 3;

    Parser parser;
    bool parsed = parser.parse(argc, const_cast<char**>(argv));

    ASSERT_TRUE(parsed);
    ASSERT_EQ(8080, parser.get_port());
    return true;
}

// Test: Parse with help flag
bool test_parser_help_flag() {
    const char* argv[] = {"pacprism", "--help"};
    int argc = 2;

    Parser parser;
    bool parsed = parser.parse(argc, const_cast<char**>(argv));

    // Help should exit, so parse should return false
    ASSERT_FALSE(parsed);
    return true;
}

// Test: Parse with invalid port (non-numeric)
bool test_parser_invalid_port() {
    const char* argv[] = {"pacprism", "--port", "invalid"};
    int argc = 3;

    Parser parser;
    bool parsed = parser.parse(argc, const_cast<char**>(argv));

    ASSERT_FALSE(parsed);
    return true;
}

// Test: Parse with out of range port
bool test_parser_out_of_range_port() {
    const char* argv[] = {"pacprism", "--port", "99999"};
    int argc = 3;

    Parser parser;
    bool parsed = parser.parse(argc, const_cast<char**>(argv));

    ASSERT_FALSE(parsed);
    return true;
}

// Test: Get default config path
bool test_parser_default_config_path() {
    const char* argv[] = {"pacprism"};
    int argc = 1;

    Parser parser;
    parser.parse(argc, const_cast<char**>(argv));

    std::string config_path = parser.get_config_path();
    ASSERT_FALSE(config_path.empty());
    return true;
}

// Run all parser tests
void run_parser_tests() {
    test::TestSuite suite("Parser Tests");

    suite.add_test("Parser: Initialization", test_parser_initialization);
    suite.add_test("Parser: Default arguments", test_parser_default_args);
    suite.add_test("Parser: Custom port", test_parser_custom_port);
    suite.add_test("Parser: Help flag", test_parser_help_flag);
    suite.add_test("Parser: Invalid port", test_parser_invalid_port);
    suite.add_test("Parser: Out of range port", test_parser_out_of_range_port);
    suite.add_test("Parser: Default config path", test_parser_default_config_path);

    suite.run();
}
