#include <memory>

#include <network/router/router.h>
#include <pacPrism/version.h>


std::shared_ptr<http::response<http::string_body>> Router::route_operation(const http::request<http::string_body>& request) {
    // Get the custom header "Operation".
    // If no Operation header, send a default response.
    std::string operation;
    auto it = request.find("Operation");

    if (it != request.end()) {
        operation = it->value();

        // Route based on operation type
        if (operation == "store") {
            return operation_store(request);
        } else {
            // Handle other operations
            auto response = std::make_shared<http::response<http::string_body>>(http::status::ok, request.version());

            std::string server = "pacPrism/";
            server.append(pacprism::getVersionFull());

            response->set(http::field::server, server);
            response->set(http::field::content_type, "text/plain");
            response->body() = "Operation: " + operation + " (not implemented yet)";
            response->prepare_payload();

            return response;
        }
    } else {
        // Create a default response and call response_sender.
        auto response = std::make_shared<http::response<http::string_body>>(http::status::ok, request.version());

        // Define server header.
        std::string server = "pacPrism/";
        server.append(pacprism::getVersionFull());

        // Define body header.
        std::string body = "Hello from pacPrism!\n";
        body.append("Version ").append(pacprism::getVersionFull()).append("\n").append("Build ").append(pacprism::getBuildInfo()).append("\n");

        // Make response.
        response->set(http::field::server, server);
        response->set(http::field::content_type, "text/plain");
        response->body() = "Hello from pacPrism!";
        response->prepare_payload();

        return response;
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