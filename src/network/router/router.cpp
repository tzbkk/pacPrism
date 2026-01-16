#include <memory>
#include <format>
#include <variant>

#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <network/router/router.hpp>
#include <pacPrism/version.h>

Router::Router(DHT_operation& dht, Validator& validator)
    : m_dht(dht), m_validator(validator) {};

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
    // TODO.
    return default_response_builder("Hello from pacPrism!", request.version(), http::status::ok);
}

router_response Router::default_response_builder(const std::string& body_string, size_t version, http::status status) {
    auto response = std::make_shared<http::response<http::string_body>>(status, version);
    response->body() = body_string;
    response->set("server", std::format("pacPrism/{}", pacprism::getVersionFull()));
    response->prepare_payload();
    return response;
}
