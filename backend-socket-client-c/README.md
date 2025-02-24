Socket Server
Socket Server is a C++ application that manages WebSocket connections for traders (on port 8090), communicates with an Order Management System (OMS) (on port 8081), and in the future, a Price Engine (on port 8080) for real-time market data. The OMS connection may also be replaced or augmented by Kafka.

Port 8090: Accepts trader connections
Port 8081: OMS integration via WebSocket
Port 8080: Future Price Engine integration
It uses vcpkg in manifest mode for dependency management and CMake for building.

Table of Contents
Overview
Architecture
Features and Constraints
Directory Structure
Building the Project
Prerequisites
The build-all.sh Script
Debugging in VS Code
Usage
Next Steps
License
Overview
The Socket Server acts as a bridge:

Trader (User) WebSockets: It listens on port 8090 for incoming connections.
OMS Connection: It establishes a WebSocket connection to ws://127.0.0.1:8085 and forwards orders (FlatBuffer data) from users to the OMS. OMS replies (JSON) are routed back to the correct user.
Price Engine: In the future, the server will also connect to ws://127.0.0.1:8080 (or another endpoint) to subscribe to real-time market data and distribute it to users.
Kafka: Eventually, the OMS link may be replaced or complemented by Kafka-based order flow.
Architecture
yaml
Copy
Edit
┌──────────────────┐      ┌────────────────┐      ┌─────────────────┐
│    Traders       │ <--> │ Socket Server  │ <--> │ OMS (port 8085) │
│ (port 8090)      │      │ (this project) │      └─────────────────┘
└──────────────────┘             |
             | (future) Price Engine (port 8080)
             | (future) Kafka
Traders send JSON or binary FlatBuffer messages.
Socket Server forwards binary data to OMS, receives JSON replies, and routes them back to each user session.
For the Price Engine, once integrated, users can subscribe/unsubscribe to symbols, up to 50 simultaneously.
Each user is limited to 10 order requests per minute (to be implemented).
Features and Constraints
WebSocket for traders on 8090
WebSocket client to OMS on 8081
Planned Price Engine on 8080
Rate limits: 50 symbol subscriptions, 10 orders/min.
vcpkg in manifest mode for dependencies like boost, spdlog, nlohmann/json, etc.
CMake build system (C++14 or higher).
Directory Structure
A possible layout (simplified):

python
Copy
Edit
.
├── build/                   # CMake build output
├── config/                  # Config files
├── logs/                    # Logging output
├── scripts/                 # Scripts (including build-all.sh)
├── Src/
│   ├── WebSocket/
│   │   ├── WebSocketServer.cpp
│   │   └── WebSocketSession.cpp
│   └── main.cpp
├── tests/                   # Unit tests (if any)
├── vcpkg/                   # vcpkg (downloaded if build-all.sh runs it)
├── vcpkg.json               # Manifest file with dependencies
├── CMakeLists.txt           # Main CMake config
├── build-all.sh             # Script to download vcpkg, bootstrap, and build
└── README.md                # This file
Building the Project
Prerequisites
Git, Bash, CMake (3.10+ recommended), a C++14 (or higher) compiler, and Python (optional, if needed by vcpkg).
vcpkg in manifest mode. This can be automated by build-all.sh.
The build-all.sh Script
We provide a single script that handles:

Downloading vcpkg if not already present.
Bootstrapping vcpkg.
Running CMake with the vcpkg toolchain in manifest mode.
Compiling the project.
Usage:

bash
Copy
Edit
# Make sure the script is executable:
chmod +x build-all.sh

# Run it:
./build-all.sh
After completion, you’ll have a build/ folder containing the compiled backend-socket-server (or similar binary).

Debugging in VS Code
If you use Visual Studio Code, we have a sample .vscode/launch.json with two debug configurations:

"Debug backend-socket-server" – does a clean build before debugging.
"Fast Debug backend-socket-server" – does a faster, incremental build before debugging.
Example snippet:

jsonc
Copy
Edit
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug backend-socket-server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/backend-socket-server",
            "preLaunchTask": "clean build",
            // ...
        },
        {
            "name": "Fast Debug backend-socket-server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/backend-socket-server",
            "preLaunchTask": "fast build",
            // ...
        }
    ]
}
Open the Run and Debug panel in VS Code, select one of these configurations, and start debugging. Adjust paths, tasks, or other settings to fit your environment.

Usage
After building:

bash
Copy
Edit
cd build
./backend-socket-server
The Socket Server starts on port 8090 for trader WebSocket connections.
It automatically attempts to connect to an OMS on 127.0.0.1:8081 (WebSocket).
In the future, it will connect to the Price Engine on 127.0.0.1:8080 for price data.
Trader-Side Usage
Connect via WebSocket to ws://<server-ip>:8090.
Send binary FlatBuffer data to place orders or text JSON to subscribe to symbols, get user data, etc.
The server routes these to the OMS or Price Engine, then replies with any JSON responses.
OMS-Side Usage
The server acts as a client to ws://127.0.0.1:8081.
OMS can push JSON updates (order statuses, position data).
The Socket Server dispatches these messages to the correct user session based on userId.
Next Steps
Add Kafka: Replace or augment the OMS WebSocket with a Kafka-based order flow.
Price Engine: Implement the logic to subscribe to real-time data from ws://127.0.0.1:8080 (or a custom endpoint).
Rate Limits: Enforce a maximum of 50 symbol subscriptions per user and 10 orders/minute.
Security: Add TLS/SSL WebSockets, JWT authentication, or other methods.
Expand Monitoring/Logging with spdlog or external monitoring tools.
License
(Example) Licensed under the MIT License.
(Or whichever license you prefer.)