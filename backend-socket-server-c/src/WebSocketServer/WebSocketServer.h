#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "WebSocketSession.h"
// #include <jwt-cpp/jwt.h> // For JWT token decoding
#include "../DataStorage/DataStorage.h"

namespace beast = boost::beast;
namespace http = beast::http;

namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebSocketSession;

class WebSocketServer : public std::enable_shared_from_this<WebSocketServer>
{
public:
    WebSocketServer(net::io_context &ioc, tcp::endpoint endpoint, DataStorage &dataStorage); // Updated to accept communicator
    void broadcast(const std::string &message);                                              // Broadcast message to all sessions
    void start();
    void stop();
    
    void removeSession(const std::string &userId); // Remove a session by user ID

private:
    tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::shared_ptr<WebSocketSession>> sessions_; // ✅ Track sessions

    DataStorage &dataStorage_;        //  std::shared_ptr<WebSocketOrderHandler> orderHandler_;
    std::atomic<bool> running_{true}; // ✅ Add missing variable

    void do_accept_(); // Accept new WebSocket connections
};

#endif // WEBSOCKET_SERVER_H
