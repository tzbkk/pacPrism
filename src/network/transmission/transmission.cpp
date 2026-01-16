#include <iostream>
#include <sstream>
#include <memory>
#include <array>
#include <variant>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <network/transmission/transmission.hpp>
#include <pacPrism/version.h>
#include <network/router/router.hpp>

// ServerTrans implementation
ServerTrans::ServerTrans(net::io_context& io_context, Router& router)
    : m_io_context(io_context), m_router(router) {}

void ServerTrans::start_server(const net::ip::address& address, unsigned short port) {
    using tcp = net::ip::tcp;

    auto self = shared_from_this();

    // Create an endpoint.
    tcp::endpoint endpoint(address, port);
    // Create an acceptor.
    m_acceptor = std::make_unique<tcp::acceptor> (m_io_context, endpoint);
    // Print message.
    std::cout << "Server started, listening on port " << port << std::endl;
    // Start accepting.
    self->start_accept();
}

void ServerTrans::start_accept() {
    auto self = shared_from_this();
    auto socket = std::make_shared<tcp::socket>(m_io_context);

    // Accept a connection.
    m_acceptor->async_accept(*socket, [self, socket](const boost::system::error_code& error) {
        // Prepare varibles.
        auto buffer = std::make_shared<beast::flat_buffer>();
        auto req_parser = std::make_shared<http::request_parser<http::string_body>>();
        if (!error) {
            self->read_from_connection(socket, buffer, req_parser);
            self->start_accept();
        } else {
            std::cerr << "Accept error: " << error.message() << std::endl;
        }
    });
}

void ServerTrans::read_from_connection(std::shared_ptr<tcp::socket> socket,
                                       std::shared_ptr<beast::flat_buffer> buffer,
                                       std::shared_ptr<http::request_parser<http::string_body>> req_parser) {

    auto self = shared_from_this();

    // Set the parser body limit (1MB)
    req_parser->body_limit(1024 * 1024);

    http::async_read(*socket, *buffer, *req_parser,
        [self, socket, buffer, req_parser](const boost::system::error_code& error, size_t bytes_transferred) {
            if (!error) {
                // Request parsing complete, process it
                auto request = req_parser->release();
                self->response_builder(socket, request);
            } else {
                if (error != http::error::end_of_stream) {
                    std::cout << "Read from " << socket->remote_endpoint() << " failed, error: " << error.message() << std::endl;
                }
                socket->close();
                return;
            }
        });
}

void ServerTrans::response_builder(std::shared_ptr<tcp::socket> socket, const http::request<http::string_body>& request) {
    auto self = shared_from_this();

    // Route.
    auto response = m_router.global_router(request);

    // Send the response.
    self->response_sender(socket, response);
}

void ServerTrans::response_sender(std::shared_ptr<tcp::socket> socket, router_response response) {
    auto self = shared_from_this();

    std::visit([self, socket](auto&& concrete_response) {
            http::async_write(*socket, *concrete_response,
        [self, socket, concrete_response](const boost::system::error_code& error, size_t bytes_transferred) {
            if (error) return;

            // Whether to keep alive.
            if (concrete_response->keep_alive()) {
                auto new_buffer = std::make_shared<beast::flat_buffer>();
                auto new_parser = std::make_shared<http::request_parser<http::string_body>>();
                self->read_from_connection(socket, new_buffer, new_parser);
            } else {
                // Shutdown connection if there is no need to keep alive.
                boost::system::error_code ec;
                socket->shutdown(tcp::socket::shutdown_send, ec);
                socket->close();
            }
        });
    }, response);
}

// ClientTrans implementation
ClientTrans::ClientTrans(net::io_context& io_context)
    : m_io_context(io_context) {}

void ClientTrans::start_connecting() {
    // TODO: Implement client connection logic
}