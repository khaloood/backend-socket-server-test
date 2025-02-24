#include "WebSocketSession.h"
#include <iostream>
#include "UserSessionManager.h"

WebSocketSession::WebSocketSession(tcp::socket socket, DataStorage &dataStorage, const std::string &userId)
    : ws_(std::move(socket)),
      dataStorage_(dataStorage),
      userId_(userId),           // Assign the userId here
      timer_(ws_.get_executor()) // Correctly initialize the timer
{
}

void WebSocketSession::start()
{
    spdlog::info("Starting WebSocket session...");

    // Disable Nagle’s Algorithm (TCP_NODELAY) for lower latency
    boost::asio::ip::tcp::no_delay option(true);
    ws_.next_layer().set_option(option);

    // Perform the WebSocket handshake asynchronously
    ws_.async_accept([self = shared_from_this()](beast::error_code ec)
                     {
        if (ec) {
            self->fail(ec, "WebSocket handshake failed");
            return;
        }
        spdlog::info("WebSocket handshake successful");

   
        self->read(); });
}


void WebSocketSession::on_close()
{
    spdlog::warn("Closing WebSocket session for user: {}", userId_);
    beast::error_code ec;
    ws_.close(websocket::close_code::normal, ec);  // ✅ Graceful WebSocket closure

    if (ec)
    {
        spdlog::error("Error closing WebSocket: {}", ec.message());
    }
}

void WebSocketSession::send(const std::string &message)
{
    ws_.text(true); // Ensure the message is sent as text
    ws_.async_write(net::buffer(message),
                    [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred)
                    {
                        boost::ignore_unused(bytes_transferred);
                        if (ec)
                        {
                            spdlog::error("Write failed: {}", ec.message());
                        }
                    });
}

void WebSocketSession::sendResponse(const std::string &responseMessage)
{
 
    ws_.text(true); // Ensure the message is sent as text
    ws_.async_write(net::buffer(responseMessage),
                    [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred)
                    {
                        boost::ignore_unused(bytes_transferred);

                        // Remove the sent message from the queue
                        // self->messageQueue_.pop();

                        if (ec)
                        {
                            spdlog::error("Failed to send message: {}", ec.message());
                        }

                        // After sending, try to send the next message
                        //  self->sendNextMessage();
                    });
}

void WebSocketSession::sendResponse(double responseMessage)
{
    // spdlog::info("Enqueueing response: {}", responseMessage);
    enqueueMessage(responseMessage);
}

void WebSocketSession::enqueueMessage(double responseMessage)
{
    messageQueue_.push(responseMessage);

    // If not currently sending a message, send the next one
    if (!sending_)
    {
        sendNextMessage();
    }
}
void WebSocketSession::sendNextMessage()
{
    if (messageQueue_.empty())
    {
        sending_ = false;
        return;
    }

    sending_ = true; // Mark that we are currently sending a message

    double message = messageQueue_.front();
    messageQueue_.pop(); // Remove from queue

    std::vector<uint8_t> buffer(sizeof(double));
    std::memcpy(buffer.data(), &message, sizeof(double)); // Convert to raw bytes

    ws_.binary(true); // Ensure binary mode
    ws_.async_write(net::buffer(buffer),
                    [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred)
                    {
                        boost::ignore_unused(bytes_transferred);

                        if (ec)
                        {
                            spdlog::error("Failed to send binary response: {}", ec.message());
                        }

                        // Try sending next message
                        self->sendNextMessage();
                    });
}


void WebSocketSession::fail(beast::error_code ec, const char *what)
{
    if (ec == websocket::error::closed || ec == boost::asio::error::connection_reset)
    {
        spdlog::warn("{}: {} (Client Disconnected)", what, ec.message());
        return;
    }

    spdlog::error("{}: {}", what, ec.message());
}


void WebSocketSession::read()
{
    ws_.async_read(buffer_, [self = shared_from_this(), this](beast::error_code ec, std::size_t bytes_transferred)
                   {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            if (ec == websocket::error::closed || ec == boost::asio::error::connection_reset)
            {
                spdlog::warn("Client {} disconnected: {}", self->userId_, ec.message());
                self->on_close(); // Properly remove session
                return;
            }
            else
            {
                self->fail(ec, "Read failed");
                return;
            }
        }

        if (self->ws_.got_binary()) {
            spdlog::info("Received binary message of size: {}", bytes_transferred);

            auto data = self->buffer_.data();
            std::vector<uint8_t> binaryData(boost::asio::buffer_cast<const uint8_t *>(data),
                                            boost::asio::buffer_cast<const uint8_t *>(data) + boost::asio::buffer_size(data));

            spdlog::info("Received binary data size: {}", binaryData.size());
            if (binaryData.size() > sizeof(uint16_t))
            {
                spdlog::warn("Incoming data out of range");
                self->sendResponse("Incoming data out of range");
                self->buffer_.consume(self->buffer_.size());

                self->read(); // Continue reading for new messages

                return;
            }

            if (!binaryData.empty()) {
                uint16_t receivedNumber;
                std::memcpy(&receivedNumber, binaryData.data(), sizeof(uint16_t));
            
                // ✅ Convert from network byte order (big-endian) to host-endian (little-endian)
                receivedNumber = boost::endian::little_to_native(receivedNumber);
            
                spdlog::warn("Extracted number (corrected): {}", receivedNumber);
            
                if (receivedNumber > 1023) {
                    spdlog::warn("Incoming data out of range (should be 0-1023)");
                    self->sendResponse("Incoming data out of range (should be 0-1023)");
                } else {
                    double avgSquare = self->dataStorage_.processNumber(receivedNumber);
                    spdlog::info("avgSquare: {}", avgSquare);
                    self->sendResponse(avgSquare);
                }
            }
        } else {
            spdlog::warn("Invalid message type, expected binary");
            self->sendResponse("Invalid message type, expected binary");
        }

        // ✅ Clear buffer after processing to prevent memory growth
        self->buffer_.consume(self->buffer_.size());

        // ✅ Continue reading next message
        self->read(); });
}
