package signal

import (
	"log"
	"net/http"
	"sync"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

type RoomManager struct {
	Rooms map[string]*Room
	mu    sync.RWMutex
}

func NewRoomManager() *RoomManager {
	return &RoomManager{
		Rooms: make(map[string]*Room),
	}
}

func (rm *RoomManager) HandleWebSocket(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("Upgrade error:", err)
		return
	}

	client := &Client{
		Conn: conn,
		Send: make(chan []byte, 256),
		RM:   rm,
	}

	go client.ReadPump()
	go client.WritePump()
}

func (rm *RoomManager) GetOrCreateRoom(roomID string) *Room {
	rm.mu.Lock()
	defer rm.mu.Unlock()

	if room, ok := rm.Rooms[roomID]; ok {
		return room
	}

	room := NewRoom(roomID)
	rm.Rooms[roomID] = room
	go room.Run()
	log.Printf("Created room: %s", roomID)
	return room
}

func (rm *RoomManager) DeleteRoom(roomID string) {
	rm.mu.Lock()
	defer rm.mu.Unlock()
	if _, ok := rm.Rooms[roomID]; ok {
		delete(rm.Rooms, roomID)
		log.Printf("Deleted room: %s", roomID)
	}
}
