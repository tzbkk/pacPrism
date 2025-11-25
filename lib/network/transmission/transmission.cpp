#include "network/transmission/transmission.h"
#include <iostream>
#include <asio.hpp>
#include <memory>
#include <array>

Transmission::Transmission(asio::io_context& io_context)
    : m_io_context(io_context) {}

void Transmission::start_server(const asio::ip::address& address, unsigned short port) {
    using asio::ip::tcp;
    // Create an endpoint.
    tcp::endpoint endpoint (address, port);
    // Create an acceptor.
    m_acceptor = std::make_unique<tcp::acceptor>(m_io_context, endpoint);
    // Print message.
    std::cout << "Server started, listening on port " << port << std::endl;
}

void Transmission::start_accept() {
    using asio::ip::tcp;
    auto socket = std::make_shared<tcp::socket>(m_io_context);

    // Use shared_from_this() to ensure object lifetime
    auto self = shared_from_this();

    m_acceptor->async_accept(*socket, [self, socket](const asio::error_code& error) {
        if (!error) {
            std::cout << "New client connected: " << socket->remote_endpoint().address().to_string() << std::endl;
            self->handle_client_connection(socket);
            self->start_accept();
        } else {
            std::cerr << "Accept error: " << error.message() << std::endl;
        }
    });
}

void Transmission::handle_client_connection(std::shared_ptr<asio::ip::tcp::socket> socket) {
    using asio::ip::tcp;

    std::cout << "Handling connection from: "
              << socket->remote_endpoint().address().to_string()
              << ":" << socket->remote_endpoint().port() << std::endl;

    auto buffer = std::make_shared<std::array<char, 1024>>();
    auto self = shared_from_this();

    socket->async_read_some(asio::buffer(*buffer), [self, socket, buffer](const asio::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            std::cout << "Received " << bytes_transferred << " bytes" << std::endl;

            std::string response = "Message received";
            socket->async_write_some(asio::buffer(response), [self, socket, buffer](const asio::error_code& write_error, std::size_t /*bytes_written*/) {
                if (!write_error) {
                    std::cout << "Response sent successfully" << std::endl;
                } else {
                    std::cerr << "Write error: " << write_error.message() << std::endl;
                }
            });
        } else if (error != asio::error::eof) {
            std::cerr << "Read error: " << error.message() << std::endl;
        }
    });
}