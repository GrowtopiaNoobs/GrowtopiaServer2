/**********************************************************************************
	New Growtopia Private Server
	Copyright (C) 2019  Growtopia Noobs
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.
	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************************/


#include "ENetServer.hpp"

short ENetServer::id_generator = 0;

void ENet_Init() {
	if (enet_initialize() != 0)
	{
		throw;
	}
	atexit(enet_deinitialize);
}

std::thread& ENetServer::getThread() {
	return this->t;
}

void ENetServer::run() {
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (true) {
		while (enet_host_service(server, &event, 10) > 0) {
			Peer peer(event.peer, id);
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					event.peer->data = NULL;
					/* Store any relevant client information here. */
					OnConnect(peer);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					OnRecievedPacket(peer, event.packet->data, event.packet->dataLength);
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					OnDisconnect(peer);
					/* Reset the peer's client information. */
					if((char *)event.peer->data != NULL) {
						delete (char *)event.peer->data;
						event.peer->data = NULL;
					}
			}
		}
		OnPeriodicEvent();
	}
}

void ENetServer::init(int port, int maxPeers) {
	if (this->server != NULL) throw;
	ENetAddress address;

	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address /* the address to bind the server host to */,
		maxPeers		/* allow up to maxPeers clients and/or outgoing connections */,
		2			/* allow up to 2 channels to be used, 0 and 1 */,
		0			/* assume any amount of incoming bandwidth */,
		0			/* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		throw;
	}
	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);
}

void ENetServer::execute() {
	std::thread t(&ENetServer::run, this);
	this->t = move(t);
}

ENetServer::ENetServer() {
	this->server = NULL;
	auto id_tpm = id_generator++;
	int str_len = sizeof(id_generator)*2;
	id.resize(str_len);
	static const char* digits = "0123456789ABCDEF";
	for (unsigned int i=0; i<str_len; ++i)
		id[i] = digits[(id_tpm>>(i<<2)) & 0x0f];
}

ENetServer::ENetServer(int port, int maxPeers) : ENetServer() {
	init(port, maxPeers);
}

ENetServer::~ENetServer() {
	enet_host_destroy(server);
}

void ENetServer::OnConnect(Peer peer)
{
	std::cout << "OnConnect is not set! forgot to override?" << std::endl;
}

void ENetServer::OnRecievedPacket(Peer peer, uint8_t* data, uint32_t dataLen)
{
	std::cout << "OnRecievedPacket is not set! forgot to override?" << std::endl;
}

void ENetServer::OnDisconnect(Peer peer)
{
	std::cout << "OnDisconnect is not set! forgot to override?" << std::endl;
}

void ENetServer::OnPeriodicEvent() {
	std::cout << "OnPeriodicEvent is not set! forgot to override?" << std::endl;
}


PeersIterator ENetServer::getPeers() {
	PeersIterator it(this->server->peers, this->server->peerCount, id);
	return it;
}

Peer ENetServer::getPeer(std::string peerID) {
	for(int i=0; i<this->server->peerCount; i++) {
		if(this->server->peers[i].state == ENET_PEER_STATE_CONNECTED) {
			if(peerID == Peer(&this->server->peers[i], id).getUID()) {
				return Peer(&this->server->peers[i], id);
			}
		}
	}
	return Peer(NULL, "");
}

void* ENetServer::getServerPtr() {
	return (void*)server;
}

std::string ENetServer::getServerID() {
	return id;
}

void ENetServer::setServerData(uint8_t* data) {
	server->data = (void*) data;
}
uint8_t* ENetServer::getServerData() {
	return (uint8_t*)server->data;
}
