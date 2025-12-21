#pragma once

#include <memory>
#include <string>

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <network/router/router.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// Forward declarations
class ServerTrans;
class ClientTrans;

// Transmission base class (pure interface)
class Transmission {
public:
    virtual ~Transmission() = default;
};

// Detailed server transmission class.
class ServerTrans : public Transmission, public std::enable_shared_from_this<ServerTrans> {
public:
    // Factory method for creating shared_ptr instances
    static std::shared_ptr<ServerTrans> create(net::io_context& io_context, Router& router) {
        return std::shared_ptr<ServerTrans>(new ServerTrans(io_context, router));
    }
    // Start a server with ip and port.
    void start_server(const net::ip::address& address, unsigned short port);
    // Start accepting.
    void start_accept();

private:
    // Private constructor for factory method
    explicit ServerTrans(net::io_context& io_context, Router& router);
    // Read individual client connections.
    void read_from_connection(std::shared_ptr<tcp::socket> socket,
                              std::shared_ptr<beast::flat_buffer> buffer,
                              std::shared_ptr<http::request_parser<http::string_body>> req_parser);
    // Build a response.
    void response_builder(std::shared_ptr<tcp::socket> socket, const http::request<http::string_body>& request);
    // Send a response.
    void response_sender(std::shared_ptr<tcp::socket> socket,
                         std::shared_ptr<http::response<http::string_body>> response);

private:
    // Member variables
    net::io_context& m_io_context;
    std::unique_ptr<tcp::acceptor> m_acceptor;
    Router& m_router;
};

// Detailed client transmission class.
class ClientTrans : public Transmission, public std::enable_shared_from_this<ClientTrans> {
public:
    // Factory method for creating shared_ptr instances
    static std::shared_ptr<ClientTrans> create(net::io_context& io_context) {
        return std::shared_ptr<ClientTrans>(new ClientTrans(io_context));
    }
    // Start connecting.
    void start_connecting();

private:
    // Private constructor for factory method
    explicit ClientTrans(net::io_context& io_context);
    // Member variables
    net::io_context& m_io_context;
};