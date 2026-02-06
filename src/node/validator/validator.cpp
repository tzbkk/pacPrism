#include <node/validator/validator.hpp>
#include <openssl/sha.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <iostream>

RequestType Validator::validate_request(const http::request<http::string_body>& request) {
    // Check if the request contains node identification headers
    auto node_id = request.find("pacPrism_node_id");
    auto node_signature = request.find("pacPrism_node_signature");

    // Case 1: Neither header present - this is a plain client request
    if (node_id == request.end() && node_signature == request.end()) {
        return RequestType::PlainClient;
    }

    // Case 2: Both headers present - validate as a node request
    if (node_id != request.end() && node_signature != request.end()) {
        // Verify the signature
        if (verify_node_identity(node_id->value(), node_signature->value(), request.body())) {
            return RequestType::Node;
        } else {
            // Signature verification failed
            return RequestType::Invalid;
        }
    }

    // Case 3: Only one header present - this is invalid
    // (either node_id without signature, or signature without node_id)
    return RequestType::Invalid;
}

bool Validator::verify_node_identity(const std::string& node_id,
                                     const std::string& node_signature,
                                     const std::string& body) {
    // Demo: always return true for now
    // TODO: Implement actual signature verification using Ed25519 or similar
    return true;
}

std::string Validator::calculate_sha256(const std::string& file_path) const {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return "";
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const size_t BUFFER_SIZE = 8192;
    char* buffer = new char[BUFFER_SIZE];

    while (file.read(buffer, BUFFER_SIZE)) {
        size_t bytes_read = file.gcount();
        if (bytes_read > 0) {
            SHA256_Update(&sha256, buffer, bytes_read);
        }
    }

    // Handle last partial read
    size_t bytes_read = file.gcount();
    if (bytes_read > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    delete[] buffer;
    file.close();

    SHA256_Final(hash, &sha256);

    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

bool Validator::verify_sha256(const std::string& file_path, const std::string& expected_hash) const {
    std::string calculated_hash = calculate_sha256(file_path);
    if (calculated_hash.empty()) {
        return false;
    }

    // Case-insensitive comparison
    if (calculated_hash.length() != expected_hash.length()) {
        return false;
    }

    for (size_t i = 0; i < calculated_hash.length(); i++) {
        if (tolower(calculated_hash[i]) != tolower(expected_hash[i])) {
            return false;
        }
    }

    return true;
}
