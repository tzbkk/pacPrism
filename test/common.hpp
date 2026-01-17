#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <functional>

// Simple test framework
namespace test {

inline int total_tests = 0;
inline int passed_tests = 0;
inline int failed_tests = 0;

// Color codes for terminal output
namespace colors {
    inline const char* RESET = "\033[0m";
    inline const char* RED = "\033[31m";
    inline const char* GREEN = "\033[32m";
    inline const char* YELLOW = "\033[33m";
    inline const char* BLUE = "\033[34m";
    inline const char* CYAN = "\033[36m";
}

// Test assertion macros
#define ASSERT_TRUE(condition) \
    do { \
        test::total_tests++; \
        if (!(condition)) { \
            std::cout << test::colors::RED << "[FAIL] " << test::colors::RESET; \
            std::cout << "Line " << __LINE__ << ": " << #condition << std::endl; \
            test::failed_tests++; \
            return false; \
        } else { \
            test::passed_tests++; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) \
    do { \
        test::total_tests++; \
        if ((expected) != (actual)) { \
            std::cout << test::colors::RED << "[FAIL] " << test::colors::RESET; \
            std::cout << "Line " << __LINE__ << ": expected " << (expected) \
                      << ", got " << (actual) << std::endl; \
            test::failed_tests++; \
            return false; \
        } else { \
            test::passed_tests++; \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        test::total_tests++; \
        if ((expected) == (actual)) { \
            std::cout << test::colors::RED << "[FAIL] " << test::colors::RESET; \
            std::cout << "Line " << __LINE__ << ": expected not equal to " \
                      << (expected) << ", got " << (actual) << std::endl; \
            test::failed_tests++; \
            return false; \
        } else { \
            test::passed_tests++; \
        } \
    } while(0)

#define ASSERT_STREQ(expected, actual) \
    do { \
        test::total_tests++; \
        if ((expected) != (actual)) { \
            std::cout << test::colors::RED << "[FAIL] " << test::colors::RESET; \
            std::cout << "Line " << __LINE__ << ": expected \"" << (expected) \
                      << "\", got \"" << (actual) << "\"" << std::endl; \
            test::failed_tests++; \
            return false; \
        } else { \
            test::passed_tests++; \
        } \
    } while(0)

#define ASSERT_STRNE(expected, actual) \
    do { \
        test::total_tests++; \
        if ((expected) == (actual)) { \
            std::cout << test::colors::RED << "[FAIL] " << test::colors::RESET; \
            std::cout << "Line " << __LINE__ << ": expected not equal to \"" \
                      << (expected) << "\"" << std::endl; \
            test::failed_tests++; \
            return false; \
        } else { \
            test::passed_tests++; \
        } \
    } while(0)

// Test case runner
struct TestCase {
    std::string name;
    std::function<bool()> func;
};

class TestSuite {
private:
    std::string suite_name_;
    std::vector<TestCase> test_cases_;

public:
    TestSuite(const std::string& name) : suite_name_(name) {}

    void add_test(const std::string& name, std::function<bool()> func) {
        test_cases_.push_back({name, func});
    }

    void run() {
        std::cout << test::colors::CYAN << "\n=== Running Test Suite: " << suite_name_
                  << " ===" << test::colors::RESET << std::endl;

        for (const auto& test_case : test_cases_) {
            std::cout << test::colors::BLUE << "Running: " << test_case.name
                      << test::colors::RESET << "... ";
            try {
                bool result = test_case.func();
                if (result) {
                    std::cout << test::colors::GREEN << "PASS" << test::colors::RESET << std::endl;
                } else {
                    std::cout << test::colors::RED << "FAIL" << test::colors::RESET << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << test::colors::RED << "EXCEPTION: " << e.what()
                          << test::colors::RESET << std::endl;
                failed_tests++;
            }
        }
    }
};

inline void print_summary() {
    std::cout << "\n" << test::colors::CYAN << "=== Test Summary ===" << test::colors::RESET << std::endl;
    std::cout << "Total tests:  " << test::total_tests << std::endl;
    std::cout << test::colors::GREEN << "Passed:       " << test::passed_tests << test::colors::RESET << std::endl;
    std::cout << test::colors::RED << "Failed:       " << test::failed_tests << test::colors::RESET << std::endl;

    if (test::failed_tests == 0) {
        std::cout << test::colors::GREEN << "\nAll tests passed!" << test::colors::RESET << std::endl;
    } else {
        std::cout << test::colors::RED << "\nSome tests failed!" << test::colors::RESET << std::endl;
    }
}

} // namespace test
