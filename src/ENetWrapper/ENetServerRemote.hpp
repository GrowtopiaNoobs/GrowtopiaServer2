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

#include "Peer.hpp"
#include "PeersIterator.hpp"
#include <enet/enet.h>

class ENetServerRemote {
	std::string id;
	ENetHost* server;

public:
	PeersIterator getPeers();
	Peer getPeer(std::string id);
	std::string getServerID();
	void* getPtr();

	ENetServerRemote(char* id, void* server) {
		this->id = id;
		this->server = (ENetHost*)server;
	}
};
