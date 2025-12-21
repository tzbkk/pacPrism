#include <memory>

#include <network/router/router.hpp>
#include <pacPrism/version.h>

#include <node/dht/dht_operation.hpp>

Router::Router(DHT_operation& dht) : m_dht(dht) {};

std::shared_ptr<http::response<http::string_body>> Router::route_operation(const http::request<http::string_body>& request) {
    // Get the custom header "Operation".
    // If no Operation header, send a default response.
    std::string operation;
    auto it = request.find("Operation");
    if (it != request.end()) {
        // TODO.
    } else {
        return this->plain_response_router(request);
    }
}

std::shared_ptr<http::response<http::string_body>> Router::operation_store(const http::request<http::string_body>& request) {
    auto response = std::make_shared<http::response<http::string_body>>(http::status::ok, request.version());

    std::string server = "pacPrism/";
    server.append(pacprism::getVersionFull());

    response->set(http::field::server, server);
    response->set(http::field::content_type, "text/plain");
    response->body() = "Operation: store (DHT storage not implemented yet)";
    response->prepare_payload();

    return response;
}

std::shared_ptr<http::response<http::string_body>> Router::plain_response_router(const http::request<http::string_body>& request) {
    // TODO.
}