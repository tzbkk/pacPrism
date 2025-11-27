#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <string>

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
    static std::shared_ptr<ServerTrans> create(asio::io_context& io_context) {
        return std::shared_ptr<ServerTrans>(new ServerTrans(io_context));
    }
    // Start a server with ip and port.
    void start_server(const asio::ip::address& address, unsigned short port);
    // Start accepting.
    void start_accept();

private:
    // Private constructor for factory method
    explicit ServerTrans(asio::io_context& io_context);
      // Read individual client connections.
    void read_from_connection(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<asio::streambuf> buffer);
    // Process data read.
    void process_from_read_data(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<asio::streambuf> buffer);

private:
    // Member variables
    asio::io_context& m_io_context;
    std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
};

// Detailed client transmission class.
class ClientTrans : public Transmission, public std::enable_shared_from_this<ClientTrans> {
public:
    // Factory method for creating shared_ptr instances
    static std::shared_ptr<ClientTrans> create(asio::io_context& io_context) {
        return std::shared_ptr<ClientTrans>(new ClientTrans(io_context));
    }
    // Start connecting.
    void start_connecting();

private:
    // Private constructor for factory method
    explicit ClientTrans(asio::io_context& io_context);
    // Member variables
    asio::io_context& m_io_context;
};

#endif // TRANSMISSION_H