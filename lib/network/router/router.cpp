#include <memory>
#include <format>
#include <variant>

#include <node/dht/dht_operation.hpp>
#include <network/router/router.hpp>
#include <pacPrism/version.h>

Router::Router(DHT_operation& dht) : m_dht(dht) {};

router_response Router::global_router(const http::request<http::string_body>& request) {
    // Check if the header contain signature and node id.
    auto node_id = request.find("pacPrism_node_id");
    auto node_signature = request.find("pacPrism_node_signature");
    if (node_id == request.end() && node_signature == request.end()) {
        // The client is not a node.
        return plain_response_router(request);
    } else if (node_id != request.end() && node_signature != request.end()) {
        // Route the request from node.
        if (verify_node_identity(node_id->value(), node_signature->value(), request.body())) return node_response_router(request);
        return default_response_builder("Node verification failed.", request.version(), http::status::forbidden);
    }
    
    // The node sent a request with wrong header.
    return default_response_builder("Invalid header.", request.version(), http::status::bad_request);
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

bool Router::verify_node_identity(const std::string& node_id, const std::string& node_signature, const std::string& body) {
    // TODO.
    return true;
}