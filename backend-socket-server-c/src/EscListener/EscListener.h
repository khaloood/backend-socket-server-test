#pragma once

#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>  // ✅ Include condition_variable
#include <spdlog/spdlog.h>
#include <boost/asio/io_context.hpp>  

#ifdef _WIN32
    #include <conio.h>
#else
    int getKey();
#endif

class EscListener {
public:
    EscListener(std::atomic<bool>& runningFlag, boost::asio::io_context& ioContext, std::condition_variable& cv);  // ✅ Pass condition_variable
    ~EscListener();  
    void start();
    void stop();

private:
    void listenForEsc();
    
    std::atomic<bool>& running;
    boost::asio::io_context& ioContext_;
    std::thread listenerThread;
    std::condition_variable& cv_;  // ✅ Store reference to condition variable
};
