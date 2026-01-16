#include <node/validator/validator.hpp>

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