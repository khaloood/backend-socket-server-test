#include "Client.h"
#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>
#include <cstring> // For memcpy()

Client::Client(const std::string &host, int port)
{
    uri_ = "ws://" + host + ":" + std::to_string(port);
    spdlog::info("Connecting to WebSocket server at {}", uri_);

    wsClient_.init_asio();

    //  Connection Opened
    wsClient_.set_open_handler([this](websocketpp::connection_hdl hdl)
                               {
        spdlog::info("Connected to server. Sending random numbers...");
        connection_ = hdl;
        sendRandomNumber(); });

    //  Message Received
    wsClient_.set_message_handler([this](websocketpp::connection_hdl hdl, tls_client::message_ptr msg)
                                  { onMessage(hdl, msg); });

 
    wsClient_.set_fail_handler([this](websocketpp::connection_hdl hdl)
                               {
                                   spdlog::error("Connection failed. Retrying in 5 seconds...");
                                   std::this_thread::sleep_for(std::chrono::seconds(5));
                                   start(); // Attempt to reconnect
                               });

    wsClient_.set_close_handler([this](websocketpp::connection_hdl hdl)
                                {
                                    spdlog::warn("Connection closed. Reconnecting...");
                                    std::this_thread::sleep_for(std::chrono::seconds(5));
                                    start(); // Attempt to reconnect
                                });

    //  Create WebSocket Connection
    websocketpp::lib::error_code ec;
    auto con = wsClient_.get_connection(uri_, ec);
    if (ec)
    {
        spdlog::error("WebSocket connection error: {}", ec.message());
        return;
    }

    wsClient_.connect(con);
}

//  Destructor to properly clean up resources
Client::~Client()
{
    stop();
}

// Handle incoming messages from server
void Client::onMessage(websocketpp::connection_hdl hdl, tls_client::message_ptr msg)
{
    auto receiveTime = std::chrono::high_resolution_clock::now();

    if (msg->get_opcode() != websocketpp::frame::opcode::binary)
    {
        spdlog::warn("Received unexpected non-binary message!");
        return;
    }

    double receivedValue;
    std::memcpy(&receivedValue, msg->get_payload().data(), sizeof(double));

    spdlog::info("Received from server (double): {:.2f}", receivedValue);

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(receiveTime - sendTime_).count();
    spdlog::info("Round-trip time: {} microseconds", duration);

    if (!running_)
        return;

    sendRandomNumber();
}

//  Prevent infinite loop and memory leak in `sendRandomNumber()`
void Client::sendRandomNumber()
{
    if (!running_)
        return;

    uint16_t number = static_cast<uint16_t>(RandomGenerator::generate(0, 1023));
    spdlog::info(" Sending number (binary): {}", number);

    websocketpp::lib::error_code ec;

    // Convert uint16_t to binary format correctly
    std::string binaryData(reinterpret_cast<const char *>(&number), sizeof(uint16_t));

    // Explicitly set the WebSocket frame opcode to `binary`
    sendTime_ = std::chrono::high_resolution_clock::now();
    wsClient_.send(connection_, binaryData, websocketpp::frame::opcode::binary, ec);

    if (ec)
    {
        spdlog::error(" Failed to send message: {}", ec.message());
    }
}

// Start WebSocket client
void Client::start()
{
    spdlog::info("Starting WebSocket client...");
    wsClient_.run();
}

// Stop WebSocket client properly
void Client::stop() {
    if (!running_)
        return;

    spdlog::info("Stopping WebSocket client...");
    running_ = false;

    websocketpp::lib::error_code ec;
    
    // Stop WebSocket Processing Before Closing
    wsClient_.stop_perpetual();

    //  Close WebSocket Safely
    wsClient_.close(connection_, websocketpp::close::status::normal, "Client shutting down", ec);

    if (ec) {
        spdlog::error("Failed to close WebSocket connection: {}", ec.message());
    }

    spdlog::info("WebSocket client stopped.");
}

