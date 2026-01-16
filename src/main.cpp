#include <iostream>

#include <boost/asio.hpp>

#include <console/banner/banner.hpp>
#include <console/io/io.hpp>
#include <console/parser/parser.hpp>
#include <network/transmission/transmission.hpp>
#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <network/router/router.hpp>

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Parser parser;
    if (!parser.parse(argc, argv)) {
        return 1;
    }

    // Print banner and version info.
    Banner banner;
    banner.print();

    // Load configuration.
    std::cout << "Loading configuration from " << parser.get_config_path() << "..." << std::endl;
    Config config;
    config.load_from_file(parser.get_config_path());
    std::string upstream = config.get_upstream();
    std::cout << "Upstream: " << upstream << std::endl;

    // Init DHT.
    std::cout << "Initing DHT..." << std::endl;
    DHT_operation dht;

    // Init validator.
    std::cout << "Initing validator..." << std::endl;
    Validator validator;

    // Init router.
    std::cout << "Initing router..." << std::endl;
    Router router(dht, validator, upstream);

    // Init server.
    std::cout << "Starting HTTP server..." << std::endl;
    try {
        // Create IO context
        boost::asio::io_context io_context;

        // Create server instance
        auto server = ServerTrans::create(io_context, router);

        // Sing up exit process.
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            std::cout << "Shutting down pacPrism..." << std::endl;
            io_context.stop();
        });

        // Start server on 0.0.0.0 with specified port
        server->start_server(boost::asio::ip::make_address("0.0.0.0"), parser.get_port());

        // Run the IO context
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
