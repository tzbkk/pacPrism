#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <asio.hpp>
#include <memory>
#include <string>

class Transmission {
    public:
        // Class constructor.
        Transmission(asio::io_context& io_context);

        // Start a server.
        void start_server(unsigned short port);
        // Connect a peer.
        void connect_to_peer(const std::string& host, unsigned short port);

    private:
        asio::io_context& io_context_;
};

#endif