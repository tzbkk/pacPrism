#include "../../common.hpp"
#include <node/validator/validator.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// Test helper: create a temporary test file
std::string create_test_file(const std::string& content) {
    std::string path = "test_temp_file.txt";
    std::ofstream file(path);
    file << content;
    file.close();
    return path;
}

// Test helper: cleanup test file
void cleanup_test_file(const std::string& path) {
    if (fs::exists(path)) {
        fs::remove(path);
    }
}

// Test: SHA256 calculation with empty file
bool test_sha256_empty_file() {
    Validator validator;
    std::string path = create_test_file("");

    std::string hash = validator.calculate_sha256(path);
    cleanup_test_file(path);

    // Known SHA256 hash of empty string
    ASSERT_STREQ("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", hash);
    return true;
}

// Test: SHA256 calculation with known content
bool test_sha256_known_content() {
    Validator validator;
    std::string content = "Hello, World!";
    std::string path = create_test_file(content);

    std::string hash = validator.calculate_sha256(path);
    cleanup_test_file(path);

    // Known SHA256 hash of "Hello, World!"
    ASSERT_STREQ("dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f", hash);
    return true;
}

// Test: SHA256 calculation with larger content
bool test_sha256_large_content() {
    Validator validator;
    std::string content(10000, 'A');  // 10000 'A' characters
    std::string path = create_test_file(content);

    std::string hash = validator.calculate_sha256(path);
    cleanup_test_file(path);

    // Hash should be 64 characters (256 bits = 64 hex chars)
    ASSERT_EQ(64, hash.length());

    // All SHA256 hashes should be lowercase hex
    for (char c : hash) {
        ASSERT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }

    return true;
}

// Test: SHA256 verification with matching hash
bool test_sha256_verify_match() {
    Validator validator;
    std::string content = "Test content for verification";
    std::string path = create_test_file(content);

    std::string hash = validator.calculate_sha256(path);
    bool verified = validator.verify_sha256(path, hash);
    cleanup_test_file(path);

    ASSERT_TRUE(verified);
    return true;
}

// Test: SHA256 verification with non-matching hash
bool test_sha256_verify_no_match() {
    Validator validator;
    std::string content = "Test content";
    std::string path = create_test_file(content);

    bool verified = validator.verify_sha256(path, "wronghash123");
    cleanup_test_file(path);

    ASSERT_FALSE(verified);
    return true;
}

// Test: SHA256 verification with case-insensitive comparison
bool test_sha256_verify_case_insensitive() {
    Validator validator;
    std::string content = "Case insensitive test";
    std::string path = create_test_file(content);

    std::string hash = validator.calculate_sha256(path);
    std::string uppercase_hash;
    for (char c : hash) {
        uppercase_hash += toupper(c);
    }

    bool verified = validator.verify_sha256(path, uppercase_hash);
    cleanup_test_file(path);

    ASSERT_TRUE(verified);
    return true;
}

// Test: SHA256 calculation with non-existent file
bool test_sha256_nonexistent_file() {
    Validator validator;
    std::string hash = validator.calculate_sha256("nonexistent_file_12345.txt");

    ASSERT_STREQ("", hash);
    return true;
}

// Test: Request validation - plain client (no headers)
bool test_request_validate_plain_client() {
    Validator validator;

    // Create a simple HTTP request without node headers
    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/test");
    request.set(http::field::host, "localhost");

    RequestType type = validator.validate_request(request);
    ASSERT_TRUE(type == RequestType::PlainClient);
    return true;
}

// Test: Request validation - node request with headers
bool test_request_validate_node() {
    Validator validator;

    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/test");
    request.set("pacPrism_node_id", "node123");
    request.set("pacPrism_node_signature", "signature123");
    request.body() = "test body";

    RequestType type = validator.validate_request(request);

    // Currently returns Node because verify_node_identity returns true (demo mode)
    ASSERT_TRUE(type == RequestType::Node);
    return true;
}

// Test: Request validation - invalid (only node_id header)
bool test_request_validate_invalid_partial_headers() {
    Validator validator;

    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/test");
    request.set("pacPrism_node_id", "node123");
    // Missing signature header

    RequestType type = validator.validate_request(request);
    ASSERT_TRUE(type == RequestType::Invalid);
    return true;
}

// Run all validator tests
void run_validator_tests() {
    test::TestSuite suite("Validator Tests");

    suite.add_test("SHA256: Empty file", test_sha256_empty_file);
    suite.add_test("SHA256: Known content", test_sha256_known_content);
    suite.add_test("SHA256: Large content", test_sha256_large_content);
    suite.add_test("SHA256: Verify match", test_sha256_verify_match);
    suite.add_test("SHA256: Verify no match", test_sha256_verify_no_match);
    suite.add_test("SHA256: Case insensitive", test_sha256_verify_case_insensitive);
    suite.add_test("SHA256: Non-existent file", test_sha256_nonexistent_file);
    suite.add_test("Request: Plain client", test_request_validate_plain_client);
    suite.add_test("Request: Node request", test_request_validate_node);
    suite.add_test("Request: Invalid partial headers", test_request_validate_invalid_partial_headers);

    suite.run();
}
