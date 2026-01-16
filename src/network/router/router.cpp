#include <memory>
#include <format>
#include <variant>

#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <network/router/router.hpp>
#include <pacPrism/version.h>

Router::Router(DHT_operation& dht, Validator& validator, const std::string& upstream)
    : m_dht(dht), m_validator(validator), m_upstream(upstream) {};

router_response Router::global_router(const http::request<http::string_body>& request) {
    // Delegate validation to Validator
    auto request_type = m_validator.validate_request(request);

    // Route based on validated request type
    switch (request_type) {
        case RequestType::PlainClient:
            return plain_response_router(request);
        case RequestType::Node:
            return node_response_router(request);
        case RequestType::Invalid:
            return default_response_builder("Invalid request.", request.version(), http::status::bad_request);
        default:
            return default_response_builder("Internal server error.", request.version(), http::status::internal_server_error);
    }
}

router_response Router::node_response_router(const http::request<http::string_body>& request) {
    // TODO.
    return default_response_builder("Node response not implemented yet.", request.version(), http::status::ok);
}

router_response Router::plain_response_router(const http::request<http::string_body>& request) {
    // Get the request target (path + query)
    std::string request_target = request.target();

    // Check if request has 'target' parameter in query string
    std::string target;
    auto target_pos = request_target.find('?');
    if (target_pos != std::string::npos) {
        // Parse query string for target parameter
        std::string query_string = request_target.substr(target_pos + 1);
        size_t target_start = query_string.find("target=");
        if (target_start != std::string::npos) {
            target_start += 7; // Skip "target="
            size_t target_end = query_string.find('&', target_start);
            if (target_end == std::string::npos) {
                target = query_string.substr(target_start);
            } else {
                target = query_string.substr(target_start, target_end - target_start);
            }
        }
    }

    // If target parameter exists, redirect to upstream
    if (!target.empty()) {
        // Construct redirect URL: http://upstream/target
        std::string redirect_url = std::format("http://{}/{}", m_upstream, target);
        return redirect_builder(redirect_url, request.version());
    }

    // No target parameter in query string
    // Check if the request path is not just "/" (i.e., it's a file request)
    std::string path = (target_pos != std::string::npos) ? request_target.substr(0, target_pos) : request_target;

    if (path != "/" && !path.empty()) {
        // This is a direct file path request (e.g., /debian/pool/main/...)
        // Redirect the entire path to upstream
        // Remove leading slash from path to avoid double slashes in URL
        std::string clean_path = (path[0] == '/') ? path.substr(1) : path;
        std::string redirect_url = std::format("http://{}/{}", m_upstream, clean_path);
        return redirect_builder(redirect_url, request.version());
    }

    // Request is just "/" with no target parameter - return hello message
    return default_response_builder("Hello from pacPrism!", request.version(), http::status::ok);
}

router_response Router::default_response_builder(const std::string& body_string, size_t version, http::status status) {
    auto response = std::make_shared<http::response<http::string_body>>(status, version);
    response->body() = body_string;
    response->set("server", std::format("pacPrism/{}", pacprism::getVersionFull()));
    response->prepare_payload();
    return response;
}

router_response Router::redirect_builder(const std::string& location, size_t version) {
    auto response = std::make_shared<http::response<http::empty_body>>(http::status::temporary_redirect, version);
    response->set("server", std::format("pacPrism/{}", pacprism::getVersionFull()));
    response->set(http::field::location, location);
    response->prepare_payload();
    return response;
}
