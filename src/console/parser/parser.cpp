#include <iostream>
#include <string>
#include <cstdlib>

#include <console/parser/parser.hpp>

unsigned short Parser::parse_port(int argc, char* argv[]) const {
    // Default port
    if (argc < 2) {
        return 9001;
    }

    // Parse port from argument
    try {
        int port = std::stoi(argv[1]);
        if (port < 1 || port > 65535) {
            std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
            print_usage(argv[0]);
            return 0;
        }
        return static_cast<unsigned short>(port);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid port number '" << argv[1] << "'" << std::endl;
        print_usage(argv[0]);
        return 0;
    }
}

void Parser::print_usage(const char* program_name) const {
    std::cout << "Usage: " << program_name << " [port]\n"
              << "  port    Optional. Port number to listen on (default: 9001)\n"
              << std::endl;
}

bool Parser::parse(int argc, char* argv[]) {
    unsigned short port = parse_port(argc, argv);
    if (port == 0) {
        return false;
    }
    m_port = port;
    return true;
}