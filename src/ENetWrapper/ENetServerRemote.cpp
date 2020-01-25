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


#include "ENetServerRemote.hpp"

PeersIterator ENetServerRemote::getPeers() {
	PeersIterator it(this->server->peers, this->server->peerCount, id);
	return it;
}

Peer ENetServerRemote::getPeer(std::string peerID) {
	for(int i=0; i<this->server->peerCount; i++) {
		if(this->server->peers[i].state == ENET_PEER_STATE_CONNECTED) {
			if(peerID == Peer(&this->server->peers[i], id).getUID()) {
				return Peer(&this->server->peers[i], id);
			}
		}
	}
	return Peer(NULL, "");
}

std::string ENetServerRemote::getServerID() {
	return id;
}

void* ENetServerRemote::getPtr() {
	return (void*)this;
}
