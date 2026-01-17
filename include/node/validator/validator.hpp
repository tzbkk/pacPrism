#pragma once

#include <boost/beast.hpp>
#include <string>
#include <filesystem>

namespace beast = boost::beast;
namespace http = beast::http;
namespace fs = std::filesystem;

// Request type classification
enum class RequestType {
    PlainClient,    // Regular client request (no node headers)
    Node,           // Validated node request (has valid headers and signature)
    Invalid         // Invalid request (missing required headers or malformed)
};

class Validator {
public:
    Validator() = default;

    // Validate request and classify its type
    // Checks for required headers and their validity
    RequestType validate_request(const http::request<http::string_body>& request);

    // SHA256 checksum methods for file integrity verification
    // Calculate SHA256 hash of a file
    // Returns hex-encoded hash string (64 characters), or empty string on failure
    std::string calculate_sha256(const std::string& file_path) const;

    // Verify file integrity by comparing calculated hash with expected hash
    // Returns true if hashes match (case-insensitive), false otherwise
    bool verify_sha256(const std::string& file_path, const std::string& expected_hash) const;

private:
    // Verify node identity signature
    bool verify_node_identity(const std::string& node_id,
                              const std::string& node_signature,
                              const std::string& body);
};