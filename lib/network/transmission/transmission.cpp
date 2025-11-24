#include "network/transmission/transmission.h"
#include <iostream>
#include <asio.hpp>

Transmission::Transmission(asio::io_context& io_context)
    : io_context_(io_context) {
}

void Transmission::start_server(unsigned short port) {
    std::cout << "Starting server on port " << port << std::endl;
    // TODO: Implement server functionality using asio
}

void Transmission::connect_to_peer(const std::string& host, unsigned short port) {
    std::cout << "Connecting to peer at " << host << ":" << port << std::endl;
    // TODO: Implement client functionality using asio
}