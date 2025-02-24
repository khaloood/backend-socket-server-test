# Use Fedora as base image
FROM fedora:latest

# Install essential build tools and dependencies
RUN dnf install -y \
    cmake \
    make \
    g++ \
    git \
    which \
    tar \
    curl \
    zip \
    unzip \
    perl \
    perl-IPC-Cmd \
    kernel-headers \
    meson \
    ninja-build \
    pkg-config \
    glib2-devel \
    gettext-devel

# Set working directory
WORKDIR /app

# Copy the entire project (including vcpkg.json)
COPY . .

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    cd /opt/vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install

# Set VCPKG environment variables
ENV VCPKG_ROOT=/opt/vcpkg
ENV CMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

# Install dependencies using vcpkg manifest mode
RUN /opt/vcpkg/vcpkg install --triplet x64-linux

# Run CMake to configure and build the project
RUN cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} && \
    cmake --build build --target backend-socket-server

# Expose WebSocket server port
EXPOSE 8080

# Start the WebSocket server
CMD ["./build/backend-socket-server"]
