package signal

import (
	"log"
)

type BroadcastMessage struct {
	Data   []byte
	Sender *Client
}

type Room struct {
	ID         string
	Clients    map[*Client]bool
	Broadcast  chan BroadcastMessage
	Register   chan *Client
	Unregister chan *Client
}

func NewRoom(id string) *Room {
	return &Room{
		ID:         id,
		Clients:    make(map[*Client]bool),
		Broadcast:  make(chan BroadcastMessage),
		Register:   make(chan *Client),
		Unregister: make(chan *Client),
	}
}

func (r *Room) Run() {
	for {
		select {
		case client := <-r.Register:
			// Notify existing clients about the new peer
			newPeerMsg := BroadcastMessage{
				Data:   []byte(`{"type": "new-peer"}`),
				Sender: client, // Sender is the new client, so they won't receive this (based on our Broadcast logic)
			}
			// We need to send this to *others*. Our Broadcast channel logic in this loop handles "sender exclusion".
			// So sending to r.Broadcast should work IF we do it *after* adding the client?
			// No, if we want to notify *others*, we can use the Broadcast channel.
			// However, the Broadcast loop iterates over r.Clients.
			// If we add the client first, they are in r.Clients.
			// Our Broadcast logic excludes the sender.

			// So:
			// 1. Add client to r.Clients
			r.Clients[client] = true
			log.Printf("Client joined room %s", r.ID)

			// 2. Broadcast "new-peer" with Sender = client.
			// The loop will skip 'client' and send to everyone else.
			go func() { r.Broadcast <- newPeerMsg }()
		case client := <-r.Unregister:
			if _, ok := r.Clients[client]; ok {
				delete(r.Clients, client)
				close(client.Send)
				log.Printf("Client left room %s", r.ID)
			}
		case message := <-r.Broadcast:
			for client := range r.Clients {
				if client != message.Sender {
					select {
					case client.Send <- message.Data:
					default:
						close(client.Send)
						delete(r.Clients, client)
					}
				}
			}
		}
	}
}
