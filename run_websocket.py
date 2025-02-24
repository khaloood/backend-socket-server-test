import subprocess
import signal
import time
import os

# Define paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))  # Get script directory
SERVER_DIR = os.path.join(BASE_DIR, "backend-socket-server-c", "build")
CLIENT_DIR = os.path.join(BASE_DIR, "backend-socket-client-c", "build")

# Define executables
SERVER_EXECUTABLE = os.path.join(SERVER_DIR, "backend-socket-server")
CLIENT_EXECUTABLE = os.path.join(CLIENT_DIR, "backend-socket-client")

# Number of clients to start
NUM_CLIENTS = 20  # Change this to run more clients

# Store process references
server_process = None
client_processes = []

def kill_existing_processes():
    """Kills any previously running instances of the server and clients."""
    print("🛑 Killing all previous WebSocket server & clients...")
    subprocess.run(["pkill", "-f", SERVER_EXECUTABLE], check=False)
    subprocess.run(["pkill", "-f", CLIENT_EXECUTABLE], check=False)
    time.sleep(1)  # Allow processes to stop

def start_server():
    """Starts the WebSocket server in a new konsole window."""
    global server_process
    print("🚀 Starting WebSocket server in a new terminal...")

    server_command = ["konsole", "--noclose", "-e", f"bash -c 'cd {SERVER_DIR} && ./backend-socket-server'"]
    server_process = subprocess.Popen(server_command)

    time.sleep(2)  # Give server time to initialize

def start_clients():
    """Starts multiple WebSocket clients in separate konsole windows."""
    global client_processes
    print(f"🚀 Starting {NUM_CLIENTS} WebSocket clients...")

    for i in range(NUM_CLIENTS):
        client_command = ["konsole", "--noclose", "-e", f"bash -c 'cd {CLIENT_DIR} && ./backend-socket-client'"]
        client = subprocess.Popen(client_command)
        client_processes.append(client)
        time.sleep(0.5)  # Avoid overwhelming the server

def stop_all():
    """Gracefully terminates all running server and client processes."""
    print("\n🛑 Stopping all WebSocket server and clients...")

    # Terminate clients
    for client in client_processes:
        client.terminate()
    client_processes.clear()

    # Terminate server
    if server_process:
        server_process.terminate()

    print("✅ All WebSocket processes stopped.")

def signal_handler(sig, frame):
    """Handles Ctrl+C to stop all running processes."""
    print("\n🛑 Received termination signal. Cleaning up...")
    stop_all()
    exit(0)

if __name__ == "__main__":
    # Register Ctrl+C signal handler
    signal.signal(signal.SIGINT, signal_handler)

    try:
        kill_existing_processes()  # Ensure no previous instances are running
        start_server()
        start_clients()

        print("\n🟢 Press Ctrl+C to stop the server and clients...")

        # Keep script running
        while True:
            time.sleep(1)

    except Exception as e:
        print(f"❌ Error: {e}")
        stop_all()
