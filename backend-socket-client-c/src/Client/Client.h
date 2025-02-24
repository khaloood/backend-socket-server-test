#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <string>
#include <atomic>
#include <chrono>
#include "../RandomGenerator/RandomGenerator.h"  

using tls_client = websocketpp::client<websocketpp::config::asio_client>;

class Client {
public:
    Client(const std::string& host, int port);
    ~Client(); 

    void start();
    void stop();

private:
    void onMessage(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);
    void sendRandomNumber();

    std::string uri_;
    tls_client wsClient_;
    websocketpp::connection_hdl connection_;
    std::atomic<bool> running_{true};

    std::chrono::high_resolution_clock::time_point sendTime_;
};
