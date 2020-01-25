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

#include <enet/enet.h>
#include <thread>
#include <string>
#include <iostream>

#include "Peer.hpp"
#include "PeersIterator.hpp"

void ENet_Init();

class ENetServer {
protected:
	std::thread& getThread();
	void init(int port, int maxPeers);
	void execute();
	ENetServer();
	ENetServer(int port, int maxPeers = 256);
	~ENetServer();
private:
	static short id_generator;
	std::thread t;
	ENetHost * server;
	std::string id;

	virtual void OnConnect(Peer peer);
	virtual void OnRecievedPacket(Peer peer, uint8_t* data, uint32_t dataLen);
	virtual void OnDisconnect(Peer peer);
	virtual void OnPeriodicEvent();

	void run();
public:
	PeersIterator getPeers();
	Peer getPeer(std::string id);
	void* getServerPtr();
	std::string getServerID();
	void setServerData(uint8_t* data);
	uint8_t* getServerData();
};
