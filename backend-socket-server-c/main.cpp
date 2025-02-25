

#include <spdlog/spdlog.h>
#include <csignal>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <condition_variable>

#include "src/EscListener/EscListener.h"
#include "src/WebSocketServer/WebSocketServer.h"
#include "src/LogConfig/LogConfig.h" // Include your new logging system
#include "src/DataStorage/DataStorage.h"
#include "src/DataDumper/DataDumper.h"

std::atomic<bool> running(true);
std::condition_variable cv;
std::mutex cv_m;
boost::asio::io_context io_context;

void signalHandler(int signum)
{
    spdlog::warn("Received signal {}. Stopping the server...", signum);
    running = false;
    io_context.stop();
    cv.notify_all();
}

int main()
{
    try
    {
        const uint16_t PORT = 8086;
        setup_logging();
        spdlog::info("Initializing server...");

        // Register signal handler for graceful shutdown
        std::signal(SIGINT, signalHandler);

        DataStorage dataStorage;
        DataDumper dataDumper(dataStorage);
        dataDumper.start();

        spdlog::info("Server started on port {}", PORT);

        boost::asio::io_context ioc;
        auto work_guard = boost::asio::make_work_guard(ioc);
        auto endpoint = tcp::endpoint(boost::asio::ip::address_v4::any(), PORT);

        EscListener escListener(running, ioc, cv); // 
        escListener.start();

        spdlog::info("Starting WebSocket server...");
        auto wsServer = std::make_shared<WebSocketServer>(ioc, endpoint, dataStorage);
        wsServer->start();

        // Run io_context in the main thread
        // Run the IO context in a separate thread
        std::thread io_thread([&ioc]
                              { ioc.run(); });

        spdlog::info("Server stopped. Exiting terminal...");
        // Wait until ESC is pressed
        std::unique_lock<std::mutex> lock(cv_m);
        cv.wait(lock, []
                { return !running.load(); });

        //
        spdlog::info("Stopping server...");
        dataDumper.stop();
        escListener.stop();
        if (wsServer)
        {
            wsServer->stop(); 
        work_guard.reset(); 

       
        if (!ioc.stopped())
        {
            ioc.stop();
        }

        if (io_thread.joinable())
        {
            io_thread.join(); 
        }
        exit(0);

        spdlog::info("Server, ESC listener, and DataDumper stopped gracefully.");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Server exception: {}", e.what());
        return EXIT_FAILURE;
    }
}
