#pragma once

#include <string>

// Command line argument parser for pacPrism using cxxopts
class Parser {
public:
    Parser() = default;

    // Parse all command line arguments and configure the application
    // Returns true if parsing succeeded, false otherwise
    bool parse(int argc, char* argv[]);

    // Get the parsed port number
    unsigned short get_port() const { return m_port; }

    // Get the config file path
    std::string get_config_path() const { return m_config_path; }

private:
    unsigned short m_port = 9001;  // Default port
    std::string m_config_path = "build/config/pacprism.conf";  // Default config path
};
