#include "network/transmission/transmission.h"
#include <iostream>
#include <asio.hpp>
#include <memory>
#include <array>

// ServerTrans implementation
ServerTrans::ServerTrans(asio::io_context& io_context)
    : m_io_context(io_context) {}

void ServerTrans::start_server(const asio::ip::address& address, unsigned short port) {
    using asio::ip::tcp;
    // Create an endpoint.
    tcp::endpoint endpoint(address, port);
    // Create an acceptor.
    m_acceptor = std::make_unique<tcp::acceptor>(m_io_context, endpoint);
    // Print message.
    std::cout << "Server started, listening on port " << port << std::endl;
}

void ServerTrans::start_accept() {
    using asio::ip::tcp;
    auto socket = std::make_shared<tcp::socket>(m_io_context);

    // Use shared_from_this() to ensure object lifetime.
    auto self = shared_from_this();
    // Accept a connection.
    m_acceptor->async_accept(*socket, [self, socket](const asio::error_code& error) {
        if (!error) {
            std::cout << "New client connected: " << socket->remote_endpoint().address().to_string() << std::endl;
            auto buffer = std::make_shared<asio::streambuf>();
            self->read_from_connection(socket, buffer);
            self->start_accept();
        } else {
            std::cerr << "Accept error: " << error.message() << std::endl;
        }
    });
}

void ServerTrans::read_from_connection(std::shared_ptr<asio::ip::tcp::socket> socket,
                                       std::shared_ptr<asio::streambuf> buffer) {
    using asio::ip::tcp;

    auto self = shared_from_this();
    auto read_handler = [self, socket, buffer](const asio::error_code error, size_t read_size) {
        if (!error) {
            // Commit the read data to the buffer
            buffer->commit(read_size);
            // Continue reading from the same connection with the same buffer.
            self->read_from_connection(socket, buffer);
        } else {
            if (error == asio::error::eof) {
                std::cout << "Read from " << socket->remote_endpoint() << " ended successfully." << std::endl;
                self->process_from_read_data(socket, buffer);
            } else {
                std::cout << "Read from " << socket->remote_endpoint() << " failed, error code " << error.message() << std::endl;
            }
        }
    };

    socket->async_read_some(buffer->prepare(1024), read_handler);
}

void ServerTrans::process_from_read_data(std::shared_ptr<asio::ip::tcp::socket> socket,
                                         std::shared_ptr<asio::streambuf> buffer) {
    // TODO: Implement HTTP request parsing and processing
    // This is where we'll parse APT client requests
    std::cout << "Processing " << buffer->size() << " bytes from client" << std::endl;

    // For now, just echo back a simple response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\npacPrism Gateway\n";
    asio::write(*socket, asio::buffer(response));
}

// ClientTrans implementation
ClientTrans::ClientTrans(asio::io_context& io_context)
    : m_io_context(io_context) {}

void ClientTrans::start_connecting() {
    // TODO: Implement client connection logic
    std::cout << "Client connection started (not implemented yet)" << std::endl;
}