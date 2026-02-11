# RtcRoom

A real-time video sharing and room management application.

## Components

- **Signaling Server**: Go-based WebSocket server for room management and signaling.
- **Client**: Qt/C++ application for Windows using WebRTC.

## Prerequisites

- Go 1.18+
- Qt 6.x
- CMake 3.16+
- A C++ compiler supporting C++17

## Build Instructions

### Signaling Server (Go)

1. Navigate to the `server` directory:
   ```bash
   cd server
   ```
2. Run the server:
   ```bash
   go run .
   ```
   The server will start on `ws://localhost:8080/ws`.

### Client (Qt/C++ Windows)

1. Open the `client/CMakeLists.txt` file in Qt Creator or use CMake from the command line.
2. Ensure you have the WebRTC libraries available. Update `CMakeLists.txt` with the correct paths to your WebRTC installation if necessary.
   - You may need to set `WEBRTC_ROOT` or manually configure include/link directories in `CMakeLists.txt`.
3. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
4. Run the generated executable.

## Usage

1. Start the Signaling Server.
2. Start two instances of the Client.
3. In the Client, enter the server URL (default: `ws://localhost:8080/ws`) and click **Connect**.
4. Enter a Room ID (e.g., `room1`) and click **Join Room**.
5. The application will simulate the signaling process (Offer/Answer exchange). Video is currently a placeholder.
