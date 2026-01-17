#include <memory>
#include <format>
#include <variant>

#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <network/router/router.hpp>
#include <console/io/io.hpp>
#include <pacPrism/version.h>

Router::Router(DHT_operation& dht, Validator& validator, FileCache& cache)
    : m_dht(dht), m_validator(validator), m_cache(cache) {};

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

    // Check for Range header
    std::string range_header;
    auto range_it = request.find(http::field::range);
    if (range_it != request.end()) {
        range_header = std::string(range_it->value());
    }

    // If target parameter exists, try to serve from cache
    if (!target.empty()) {
        // Request has target parameter (e.g., /?target=/debian/pool/...)
        std::string target_path = (target[0] == '/') ? target : "/" + target;

        std::shared_ptr<http::response<http::file_body>> file_response;
        if (!range_header.empty()) {
            file_response = m_cache.get_or_fetch_with_range(target_path, request.version(), range_header);
        } else {
            file_response = m_cache.get_or_fetch(target_path, request.version());
        }

        if (file_response) {
            return file_response;
        }
        return default_response_builder("Failed to fetch file from upstream.", request.version(), http::status::bad_gateway);
    }

    // No target parameter in query string
    // Check if the request path is not just "/" (i.e., it's a file request)
    std::string path = (target_pos != std::string::npos) ? request_target.substr(0, target_pos) : request_target;

    if (path != "/" && !path.empty()) {
        // This is a direct file path request (e.g., /debian/pool/main/...)
        // Try to serve from cache

        std::shared_ptr<http::response<http::file_body>> file_response;
        if (!range_header.empty()) {
            file_response = m_cache.get_or_fetch_with_range(path, request.version(), range_header);
        } else {
            file_response = m_cache.get_or_fetch(path, request.version());
        }

        if (file_response) {
            return file_response;
        }
        return default_response_builder("Failed to fetch file from upstream.", request.version(), http::status::bad_gateway);
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
