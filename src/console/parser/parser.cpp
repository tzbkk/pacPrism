#include <iostream>
#include <cstdlib>
#include <cxxopts.hpp>

#include <console/parser/parser.hpp>

bool Parser::parse(int argc, char* argv[]) {
    try {
        cxxopts::Options options(argv[0], "pacPrism - Semi-decentralized Package Distribution System");

        // Default values
        unsigned short port = 9001;
        std::string config_path = "build/config/pacprism.conf";

        options.add_options()
            ("h,help", "Print usage information")
            ("c,config", "Path to configuration file", cxxopts::value<std::string>(config_path)->default_value("build/config/pacprism.conf"))
            ("p,port", "Port number to listen on", cxxopts::value<unsigned short>(port)->default_value("9001"))
        ;

        // Parse options
        auto result = options.parse(argc, argv);

        // Show help if requested
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return false;
        }

        // Validate port range
        if (port < 1 || port > 65535) {
            std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
            std::cout << options.help() << std::endl;
            return false;
        }

        // Set parsed values
        m_port = port;
        m_config_path = config_path;

        return true;
    } catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        return false;
    }
}