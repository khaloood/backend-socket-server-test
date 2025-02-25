#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <csignal>

#ifdef _WIN32
    #include <conio.h>
#else
    int getKey();  
#endif

class EscListener {
public:
    EscListener(std::atomic<bool>& runningFlag);
    void start();
    void stop();

private:
    void listenForEsc();
    std::atomic<bool>& running;
    std::thread listenerThread;
};
