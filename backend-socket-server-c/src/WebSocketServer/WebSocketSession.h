#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <spdlog/spdlog.h>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include "../DataStorage/DataStorage.h"
#include <queue>
// #include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
// WebSocket session class to manage individual client connections
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    // Constructor now accepts QuickFIXTradeServerCommunicator&
    explicit WebSocketSession(tcp::socket socket, DataStorage &dataStorage, const std::string &userId);

    void start();
    void on_close();
    void send(const std::string &message);
    void sendResponse(const std::string &responseMessage);
    void processOrder(const std::vector<uint8_t> &binaryData);
    void processTextMessage(const std::string &message, const std::string &userId);
    void sendResponse(double responseMessage);
    void enqueueMessage(double responseMessage);

private:
    void sendNextMessage();
    websocket::stream<tcp::socket> ws_;
    boost::asio::steady_timer timer_; // Timer for timeouts
    void read();
    void fail(beast::error_code ec, const char *what);
    std::string userId_;
    beast::flat_buffer buffer_;
    DataStorage &dataStorage_;
    // Helper function to process the validated order
    std::queue<double> messageQueue_; // Message queue to handle multiple sends
    bool sending_ = false;            // Indicates if a message is currently being sent
};

#endif // WEBSOCKET_SESSION_H
