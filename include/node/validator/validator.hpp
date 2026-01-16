#pragma once

#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

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

private:
    // Verify node identity signature
    bool verify_node_identity(const std::string& node_id,
                              const std::string& node_signature,
                              const std::string& body);
};