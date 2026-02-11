
.PHONY: all client-debug client-build server-build-linux deploy

# Default target
all: client-build server-build-linux

# Client Debug Build (Mac/Local)
client-debug:
	mkdir -p client/build_debug
	cd client/build_debug && cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_WEBRTC=OFF .. && make
	@echo "Client Debug build complete. Executable in client/build_debug/"

# Client Release Build (Mac/Local)
client-build:
	mkdir -p client/build_release
	cd client/build_release && cmake -DCMAKE_BUILD_TYPE=Release -DUSE_WEBRTC=OFF .. && make
	@echo "Client Release build complete. Setting permissions and running..."
	chmod +x client/build_release/RtcRoomClient
	# Remove quarantine attribute to allow running on Mac without "unidentified developer" warning
	xattr -d com.apple.quarantine client/build_release/RtcRoomClient || true
	./client/build_release/RtcRoomClient

# Server Cross-compile for Linux
server-build-linux:
	cd server && GOOS=linux GOARCH=amd64 go build -o rtc-room-server .
	@echo "Server compiled for Linux."

# Deploy to VPS
# User Info: IP 64.186.238.105, Key ~/.ssh/id_rsa.pem
SERVER_IP=64.186.238.105
SSH_KEY=~/.ssh/id_rsa.pem
REMOTE_DIR=/root/rtc-room

deploy-bin: server-build-linux
	@echo "Deploying binary to $(SERVER_IP)..."
	ssh -i $(SSH_KEY) root@$(SERVER_IP) "mkdir -p $(REMOTE_DIR)"
	rsync -avz -e "ssh -i $(SSH_KEY)" server/rtc-room-server root@$(SERVER_IP):$(REMOTE_DIR)/

deploy: deploy-bin
	@echo "Restarting background service on $(SERVER_IP)..."
	ssh -i $(SSH_KEY) root@$(SERVER_IP) "cd $(REMOTE_DIR) && pkill rtc-room-server || true && nohup ./rtc-room-server > server.log 2>&1 &"
	@echo "Deployment complete. Server running in background."

start: deploy-bin
	@echo "Starting server in foreground on $(SERVER_IP)..."
	ssh -t -i $(SSH_KEY) root@$(SERVER_IP) "cd $(REMOTE_DIR) && pkill rtc-room-server || true && ./rtc-room-server"
