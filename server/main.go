package main

import (
	"log"
	"net/http"
	"rtc-room/signal"
)

func main() {
	roomManager := signal.NewRoomManager()

	http.HandleFunc("/ws", roomManager.HandleWebSocket)

	log.Println("Signaling server started on :3102")
	if err := http.ListenAndServe(":3102", nil); err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
