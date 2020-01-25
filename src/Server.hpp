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


#pragma once

#define X (this->events.currentDataPtr)

#include <map>
#include <vector>
#include <string>

#include "ENetWrapper/ENetServer.hpp"
#include "Packets/PacketsDecoder/PacketsDecoder.hpp"
#include "Packets/PacketsEncoder/PacketsEncoder.hpp"
#include "Events/EventsHandler.h"

void registerServerEvents();

struct ServerData { // add variables under existing ones, so compatibility isn't broken
	char* serverUrl;
	int serverPort;
};

class Server : public ENetServer, private PacketDecoder {
private:
	EventsGlobalPool events;
	void OnConnect(Peer peer) override;
	void OnRecievedPacket(Peer peer, uint8_t* data, uint32_t dataLen) override;
	void OnDisconnect(Peer peer) override;
	void OnPeriodicEvent() override;

	void OnPlayerActionPacket(Peer peer, std::string data) override;
	void OnServerActionPacket(Peer peer, std::string data) override;
	void OnTankPacket(Peer peer, TankPacketStruct data, uint8_t* additionalData, uint32_t additionalDataSize) override;
	void OnUnknownPacket(Peer peer, int type) override;
public:
	Server() {};

	void init(int port, int peerCount, std::string url) {
		uint8_t* tmp = *events.currentDataPtr;
		ENetServer::init(port, peerCount);
		ServerData* data = new ServerData;
		char* name = new char[url.length()+1];
		memcpy(name, url.c_str(), url.length()+1);
		data->serverUrl = name;
		data->serverPort = port;
		setServerData((uint8_t*)data);
		EventPack pack(X);
		events.create_event_self("__MallocServer", pack);
		OnPeriodicEvent();
		*events.currentDataPtr = tmp;
	}

	void execute() {
		ENetServer::execute();
	}
};
