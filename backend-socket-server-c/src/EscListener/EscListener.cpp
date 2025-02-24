#include "EscListener.h"

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>

int getKey()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

EscListener::EscListener(std::atomic<bool> &runningFlag, boost::asio::io_context &ioContext, std::condition_variable &cv)
    : running(runningFlag), ioContext_(ioContext), cv_(cv) {} // ✅ Initialize cv_

EscListener::~EscListener()
{
    stop();
}

void EscListener::start()
{
    listenerThread = std::thread(&EscListener::listenForEsc, this);
}

void EscListener::stop()
{
    running = false;
    cv_.notify_all(); // ✅ Ensure `cv.wait()` in `main()` is notified
    if (!ioContext_.stopped())
    {
        ioContext_.stop();
    }

    if (listenerThread.joinable())
    {
        listenerThread.join();
    }
}

void EscListener::listenForEsc()
{
    while (running)
    {
#ifdef _WIN32
        if (_kbhit())
        {
            int key = _getch();
            if (key == 27) // ESC Key
            {
                spdlog::warn("ESC key pressed. Stopping server...");
                running = false;
                ioContext_.stop();
                cv_.notify_all(); // ✅ Correctly notify condition variable
                return;
            }
        }
#else
        int key = getKey();
        if (key == 27)
        {
            spdlog::warn("ESC key pressed. Stopping server...");
            running = false;
            ioContext_.stop();
            cv_.notify_all(); // ✅ Correctly notify condition variable
            return;
        }
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
