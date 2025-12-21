#include <iostream>

#include <boost/asio.hpp>

#include <pacPrism/version.h>
#include <network/transmission/transmission.hpp>
#include <node/dht/dht_operation.hpp>
#include <network/router/router.hpp>

int main() {
    // Print banner and version info.
    std::cout << "\033[32mpacPrism - Semi-decentralized Package Distribution System\033[0m" << std::endl;
    std::cout << "Version \033[34m" << pacprism::getVersionFull() << "\033[0m" << std::endl;
    std::cout << "Build \033[33m" << pacprism::getBuildInfo() << "\033[0m" << std::endl;
    if (pacprism::getGitInfo() != "no git info") {
        std::cout << "Git: \033[35m" << pacprism::getGitInfo() << "\033[0m" << std::endl;
    }

    // Init DHT.
    std::cout << "Initing DHT..." << std::endl;
    DHT_operation dht;

    // Init router.
    std::cout << "Initing router..." << std::endl;
    Router router;


    // Init server.
    std::cout << "Starting HTTP server..." << std::endl;
    try {
        // Create IO context
        boost::asio::io_context io_context;
        // Create server instance
        auto server = ServerTrans::create(io_context, router);

        // Start server on localhost:9001
        server->start_server(boost::asio::ip::make_address("127.0.0.1"), 9001);
        // Run the IO context
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}