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


#include "Peer.hpp"

std::string Peer::getIP() {
	if(this->peer==NULL) return "";
	std::string ret;
	int ipint = this->peer->address.host;

	ret += std::to_string(ipint & 0xFF) + ".";
	ret += std::to_string((ipint >>  8) & 0xFF) + ".";
	ret += std::to_string((ipint >> 16) & 0xFF) + ".";
	ret += std::to_string((ipint >> 24) & 0xFF);

	return ret;
}

uint32_t Peer::getIPasInt() {
	if(this->peer==NULL) return 0;
	return this->peer->address.host;
}

uint32_t Peer::getPing() { // returns ping in miliseconds
	if(this->peer==NULL) return 0;
	return this->peer->roundTripTime;
}

std::string Peer::getUID() {
	if(this->peer==NULL) return 0;
	std::string ret;
	int serverIDSize = serverID.size();
	ret.resize(serverIDSize + 8);
	unsigned int id = this->peer->connectID;
	static const char* digits = "0123456789ABCDEF";
	for (unsigned int i=0; i<serverIDSize; ++i)
		ret[i] = serverID[i];
	for (unsigned int i=0; i<8; ++i)
		ret[i+serverIDSize] = digits[(id>>(i<<2)) & 0x0f];
	return ret;
}

uint32_t Peer::getIncomingBandwidth() {
	if(this->peer==NULL) return 0;
	return this->peer->incomingBandwidth;
}

uint32_t Peer::getOutgoingBandwidth() {
	if(this->peer==NULL) return 0;
	return this->peer->outgoingBandwidth;
}

void Peer::requestDisconnect() {
	if(this->peer==NULL) return;
	enet_host_flush(peer->host);
	enet_peer_disconnect(this->peer, 0);
}

void Peer::sendPacket(int packetType, char* data, int dataLeght) {
	if(this->peer==NULL) return;
	int dataLength = dataLeght + ((data != NULL && dataLeght > 0 && data[0] == 4) ? 0 : 1);
	ENetPacket* packet = enet_packet_create(0, dataLength + 4, 1);
	*(int*)packet->data = packetType;
	if(data!=NULL)
		std::memcpy(packet->data + 4, data, dataLength);
	enet_peer_send(peer, 0, packet);
}

char* Peer::getData() { // TODO: change to uint8_t
	if(this->peer==NULL) return NULL;
	return (char*)this->peer->data;
}

void Peer::setData(char* data) {
	if(this->peer==NULL) return;
	this->peer->data = data;
}

bool Peer::exists() {
	if(this->peer) {
		return true;
	} else {
		return false;
	}
}

Peer::Peer(ENetPeer* peer, std::string serverID) {
	this->peer = peer;
	this->serverID = std::move(serverID);
}

Peer::Peer() {
	this->peer = NULL;
}
