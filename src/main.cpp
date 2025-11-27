#include <iostream>
#include <asio.hpp>
#include "network/transmission/transmission.h"
#include "node/dht/dht_operation.h"

int main() {
    std::cout << "pacPrism - Semi-decentralized Package Distribution System" << std::endl;
    std::cout << "Version Alpha 0.1.0" << std::endl;
    std::cout << "Starting HTTP server..." << std::endl;

    try {
        // Create IO context
        asio::io_context io_context;

        // Create server instance
        auto server = ServerTrans::create(io_context);

        // Start server on localhost:8080
        server->start_server(asio::ip::make_address("0.0.0.0"), 8080);

        // Run the IO context
        io_context.run();

    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}