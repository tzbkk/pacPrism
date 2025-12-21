#pragma once

#include <boost/beast.hpp>
#include <memory>

#include <node/dht/dht_operation.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    Router(DHT_operation& dht);
    // Route request by operation.
    std::shared_ptr<http::response<http::string_body>> route_operation(const http::request<http::string_body>& request);

private:
    std::shared_ptr<http::response<http::string_body>> operation_store(const http::request<http::string_body>& request);
    std::shared_ptr<http::response<http::string_body>> plain_response_router(const http::request<http::string_body>& request);
    std::shared_ptr<http::response<http::string_body>> node_response_router(const http::request<http::string_body>& request);

private:
    DHT_operation& m_dht;
};