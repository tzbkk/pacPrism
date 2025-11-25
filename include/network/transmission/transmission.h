#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <asio.hpp>
#include <memory>
#include <string>

class Transmission : public std::enable_shared_from_this<Transmission> {
public:
    // Factory method for creating shared_ptr instances
    static std::shared_ptr<Transmission> create(asio::io_context& io_context) {
        return std::shared_ptr<Transmission>(new Transmission(io_context));
    }

public:
    // Start a server with ip and port.
    void start_server(const asio::ip::address& address, unsigned short port);
    // Start accepting.
    void start_accept();

private:
    // Handle individual client connections.
    void handle_client_connection(std::shared_ptr<asio::ip::tcp::socket> socket);

    // Private constructor for factory method
    Transmission(asio::io_context& io_context);

private:
    asio::io_context& m_io_context;
    std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
};

#endif