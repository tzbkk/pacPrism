#pragma once

#include <boost/beast.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    // Route request by operation.
    std::shared_ptr<http::response<http::string_body>> route_operation(const http::request<http::string_body>& request);
private:
    std::shared_ptr<http::response<http::string_body>> operation_store(const http::request<http::string_body>& request);
};