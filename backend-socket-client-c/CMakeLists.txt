cmake_minimum_required(VERSION 3.15)
project(backend-socket-client-c)

set(CMAKE_BUILD_TYPE Debug)

# ✅ Set Compiler Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Set Vcpkg Toolchain File
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# ✅ Define Dependency Paths for Vcpkg
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/share")

# ✅ Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Src
    ${CMAKE_SOURCE_DIR}/Src/WebSocket
   # ${CMAKE_SOURCE_DIR}/Src/DataStorage


)

# ✅ Find Packages
find_package(Boost REQUIRED COMPONENTS system thread asio beast variant2)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(libwebsockets REQUIRED)
find_package(websocketpp CONFIG REQUIRED)
find_package(Threads REQUIRED)


option(ENABLE_BENCHMARK "Enable benchmark logging" ON)  # Default: OFF

if(ENABLE_BENCHMARK)
    add_definitions(-DENABLE_BENCHMARK)
    message(STATUS "Benchmarking ENABLED")
else()
    message(STATUS "Benchmarking DISABLED")
endif()

# ✅ Define Executable
add_executable(backend-socket-client
    main.cpp
    src/Client/Client.cpp
 #   src/ConnectionManager/ConnectionManager.cpp
    src/LogConfig/LogConfig.cpp
    src/EscListener/EscListener.cpp
    src/RandomGenerator/RandomGenerator.cpp

 

)

# ✅ Link Libraries
target_link_libraries(backend-socket-client PRIVATE 
    Boost::system
    Boost::thread
    Boost::asio
    Boost::beast
    Boost::variant2
    spdlog::spdlog 
    fmt::fmt
    websockets
    websocketpp::websocketpp
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Threads::Threads
)

# ✅ Ensure Dependencies Are Installed via Vcpkg
install(TARGETS backend-socket-client DESTINATION ${CMAKE_BINARY_DIR}/)
