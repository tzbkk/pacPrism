#pragma once

#include <string>

// Command line argument parser for pacPrism
class Parser {
public:
    Parser() = default;

    // Parse port from command line arguments
    // Returns 0 if parsing fails, otherwise returns the port number
    unsigned short parse_port(int argc, char* argv[]) const;

    // Print usage information
    void print_usage(const char* program_name) const;

    // Parse all command line arguments and configure the application
    // Returns true if parsing succeeded, false otherwise
    bool parse(int argc, char* argv[]);

    // Get the parsed port number
    unsigned short get_port() const { return m_port; }

private:
    unsigned short m_port = 9001;  // Default port
};