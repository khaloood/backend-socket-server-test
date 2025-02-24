#include "WebSocketServer.h"
#include "WebSocketSession.h" // Include the full definition of WebSocketSession
#include "UserSessionManager.h"
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <atomic>

static std::atomic<uint32_t> sessionCounter{0};

WebSocketServer::WebSocketServer(net::io_context &ioc, tcp::endpoint endpoint, DataStorage &dataStorage)
    : acceptor_(ioc, endpoint),
      dataStorage_(dataStorage), // Initialize communicator reference.
      running_(true)
{
}

void WebSocketServer::broadcast(const std::string &message)
{
    for (auto &[userId, session] : sessions_) // ✅ Extract session correctly
    {
        session->send(message);
    }
}

void WebSocketServer::start()
{

    do_accept_();
}
void WebSocketServer::stop()
{
    spdlog::warn("Stopping WebSocket server...");
    running_ = false;
    acceptor_.close(); // ✅ Stop accepting new connections

    // ✅ Close all active WebSocket sessions safely
    for (auto &[userId, session] : sessions_)
    {
        if (session)
        {
            try
            {
                session->on_close();
            }
            catch (const std::exception &e)
            {
                spdlog::error("Error closing WebSocket session for {}: {}", userId, e.what());
            }
        }
    }

    sessions_.clear(); // ✅ Ensure all session references are removed
}


void WebSocketServer::do_accept_()
{
    spdlog::info("Waiting for a new connection..."); // Log when waiting for a connection

    acceptor_.async_accept([self = shared_from_this()](boost::system::error_code ec, tcp::socket socket)
                           {
                               if (!ec)
                               {
                                   spdlog::info("New connection accepted from {}", socket.remote_endpoint().address().to_string());

                                   // Generate or extract a userId, for now we use a placeholder
                                   std::string userId = "trader Id : " + std::to_string(++sessionCounter);
                                   // Handle the connection with all required parameters
                                   auto session = std::make_shared<WebSocketSession>(std::move(socket), self->dataStorage_, userId);
                                   UserSessionManager::getInstance().addSession(userId, session);
                                   self->sessions_.insert({userId, session}); // ✅ Correctly insert as a key-value pair

                                   // self->sessions_.insert(session);
                                   session->start();
                               }
                               else
                               {
                                   // Log the error
                                   spdlog::error("Accept error: {}", ec.message());
                               }
                               self->do_accept_(); // Continue accepting new connections
                           });
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

void WebSocketServer::removeSession(const std::string &userId)
{
    auto it = sessions_.find(userId);
    if (it != sessions_.end())
    {
        it->second->on_close(); // ✅ Close WebSocket session before removing
        sessions_.erase(it);
    }
}
