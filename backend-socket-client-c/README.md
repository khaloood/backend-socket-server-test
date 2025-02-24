WebSocket Client

Overview

This WebSocket Client is a C++ application that connects to a WebSocket server, sends randomly generated binary numbers within a specified range, and receives a calculated response from the server. The client continuously repeats this process, demonstrating a real-time exchange of data over WebSockets.

Features

Establishes a WebSocket connection to the server.

Generates random binary numbers between a specified range (e.g., 0 to 1023).

Sends the generated binary number to the WebSocket server.

Receives a computed response from the server (e.g., an average of squares calculation).

Continuously interacts with the server in a loop.

Uses Boost.Asio for networking and Boost.Beast for WebSockets.

Uses vcpkg in manifest mode for dependency management.

WebSocket Communication Flow

The client connects to the WebSocket server.

It generates a random binary number (0-1023).

The number is sent to the WebSocket server as a binary message.

The server calculates the response (e.g., an average of squares of received numbers).

The client receives the computed value.

The client logs the received value, generates a new random number, and repeats the process.

Building the Project

This project uses vcpkg in manifest mode for dependency management and CMake for building.

Prerequisites

Ensure the following tools are installed:

C++ compiler (GCC, Clang, or MSVC)

CMake (3.10+ recommended)

Boost Libraries

vcpkg package manager

Cloning the Repository

git clone https://github.com/khaloood/backend-socket-server-test.git

Installing Dependencies via vcpkg

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install

Building the Client
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
make 