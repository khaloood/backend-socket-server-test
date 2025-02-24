#include "src/Client/Client.h"
#include "src/EscListener/EscListener.h"
#include <spdlog/spdlog.h>
#include <csignal>
#include <thread>
#include <cstdlib>  //  For `exit(0)`

std::atomic<bool> running(true);
Client* globalClient = nullptr;  //  Declare global client pointer

void signalHandler(int signum) {
    spdlog::warn("Received signal {}. Stopping the client...", signum);
    running = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Allow cleanup

    //  Use the global client instance
    if (globalClient) {
        globalClient->stop();
    }

    spdlog::info("Client shut down successfully.");
    std::exit(0);  //  Use std::exit() for clean termination
}

int main() {
    try {
        spdlog::info("Starting WebSocket client...");

        std::signal(SIGINT, signalHandler);
        Client client("127.0.0.1", 8086);
        EscListener escListener(running);  //  Create EscListener
        escListener.start();  //  Start ESC listener thread

        client.start();
        
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.stop();  
        spdlog::info("Client stopped gracefully.");

        return EXIT_SUCCESS;  // Natural exit, no need for `exit(0)`
        
    } catch (const std::exception &e) {
        spdlog::error("Client exception: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
