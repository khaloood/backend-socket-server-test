WebSocket Server (Backend-Socket-Server-C)

📜 Overview

This project is a WebSocket server built using Boost.Asio and Boost.Beast, designed to handle real-time WebSocket connections. It receives binary data, processes it, and returns responses.

⚡ Features

Uses Boost.Asio for asynchronous networking.

Supports multiple client connections.

Processes binary data efficiently.

Implements graceful shutdown using EscListener (press ESC to stop the server).

Logs all events using spdlog.

Periodically dumps data to a binary file for persistence.

📂 Project Structure

backend-socket-server-c/
│── build/                   # Compiled binaries
│── src/                     # Source code
│   ├── Benchmark/           # Macros for benchmarking execution time
│   ├── DataDumper/          # Periodically writes processed data to a file
│   ├── DataStorage/         # Stores and processes incoming data
│   ├── EscListener/         # Listens for ESC key to stop the server
│   ├── LogConfig/           # Handles log file setup
│   ├── WebSocketServer/     # WebSocket server implementation
│   │   ├── UserSessionManager.cpp   # Manages connected users
│   │   ├── WebSocketServer.cpp      # Handles new WebSocket connections
│   │   ├── WebSocketServer.h
│   │   ├── WebSocketSession.cpp     # Manages individual WebSocket clients
│   │   ├── WebSocketSession.h
│── CMakeLists.txt           # CMake build system
│── Dockerfile               # Docker build configuration
│── docker-compose.yml       # Docker multi-container configuration
│── main.cpp                 # Main entry point for the server
│── setup_project.sh         # Script for setting up the project
│── README.md                # This file
│── vcpkg/                   # vcpkg dependency manager
│── vcpkg_installed/         # Installed dependencies
│── vcpkg-configuration.json # Configuration for vcpkg
│── vcpkg.json               # vcpkg dependencies
│── log/                     # Log files
│── numbers_dump.bin         # Binary file storing dumped numbers
│── app_logs.txt             # Server logs

Prerequisites

Before running the server, ensure you have the following installed:

C++ Compiler (g++, clang++)

CMake (>= 3.10)

Boost Libraries (boost-asio, boost-beast, boost-system)

spdlog (for logging)

vcpkg (for dependency management)

Install Dependencies

On Fedora:

sudo dnf install cmake gcc-c++ boost-devel spdlog-devel

On Ubuntu/Debian:

sudo apt install cmake g++ libboost-all-dev libspdlog-dev

⚙️ Build Instructions

1️⃣ Clone the Repository

git clone https://github.com/yourusername/backend-socket-server-c.git
cd backend-socket-server-c

2️⃣ Build the Project

git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg integrate install
./vcpkg/vcpkg install

mkdir build && cd build
cmake ..
make -j$(nproc)

3️⃣ Run the Server

Navigate to the build folder and execute the server:

./backend-socket-server

Start the Server

./backend-socket-server

The server starts and listens on port 8086.

It waits for WebSocket clients to connect.

Logs will be displayed in the terminal and log file.

Stop the Server

Method 1️⃣: Press ESC Key

The server listens for an ESC key event and gracefully shuts down when pressed.

