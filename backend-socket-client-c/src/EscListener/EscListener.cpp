#include "EscListener.h"

#ifndef _WIN32
    #include <termios.h>
    #include <unistd.h>

    int getKey() {
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

EscListener::EscListener(std::atomic<bool>& runningFlag) : running(runningFlag) {}

void EscListener::start() {
    listenerThread = std::thread(&EscListener::listenForEsc, this);
}

void EscListener::stop() {
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
}

void EscListener::listenForEsc() {
    while (running) {
        #ifdef _WIN32
            if (_kbhit()) {
                int key = _getch();
                if (key == 27) {  // ASCII 27 = ESC key
                    spdlog::warn("ESC key pressed. Stopping client...");
                    running = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Prevent re-trigger
                    std::terminate();  
                }
            }
        #else
            int key = getKey();
            if (key == 27) {  // ASCII 27 = ESC key
                spdlog::warn("ESC key pressed. Stopping client...");
                running = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Prevent re-trigger
                std::terminate();  
            }
        #endif

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Non-blocking sleep
    }
}

