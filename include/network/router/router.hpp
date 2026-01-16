#pragma once

#include <memory>
#include <variant>

#include <boost/beast.hpp>

#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <console/io/io.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

using router_response = std::variant<
    std::shared_ptr<http::response<http::string_body>>,
    std::shared_ptr<http::response<http::file_body>>,
    std::shared_ptr<http::response<http::empty_body>>
>;

class Router {
public:
    Router(DHT_operation& dht, Validator& validator, const std::string& upstream);
    // Route request by operation.
    router_response global_router(const http::request<http::string_body>& request);

private:
    // Process requests from non node cilents.
    router_response plain_response_router(const http::request<http::string_body>& request);

    // Process requests from other nodes.
    router_response node_response_router(const http::request<http::string_body>& request);

    // Default response builder.
    router_response default_response_builder(const std::string& body_string, std::size_t version, http::status status);

    // Build redirect response
    router_response redirect_builder(const std::string& location, size_t version);

private:
    DHT_operation& m_dht;
    Validator& m_validator;
    std::string m_upstream;
};