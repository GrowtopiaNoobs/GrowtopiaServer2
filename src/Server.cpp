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


#include "Server.hpp"

EVENT(SendLoginRequestPacket) {
	try {
		LoginRequestPacket p;
		auto peer = pack["peer"];
		if(peer.getType() == EventPackDataType::STRING) {
			p.send(server.getPeer(peer));
		} else {
			std::vector<std::string> peers = peer;
			for(auto x: peers) {
				p.send(server.getPeer(x));
			}
		}
	} catch(...) {
		std::cout << "Missing peer for LoginReuqestPacket" << std::endl;
	}
}

EVENT(SendTankPacket) {
	std::string data;
	try {
		TankPacket p;
		try {
			p.packetType = (int)pack["packetType"];
		} catch(...) {}
		try {
			p.netID = (int)pack["netID"];
		} catch(...) {}
		try {
			p.padding1 = pack["padding1"];
		} catch(...) {}
		try {
			p.characterState = (int)pack["characterState"];
		} catch(...) {}
		try {
			p.padding2 = pack["padding2"];
		} catch(...) {}
		try {
			p.plantingTree = (int)pack["plantingTree"];
		} catch(...) {}
		try {
			p.x = (float)pack["x"];
		} catch(...) {}
		try {
			p.y = (float)pack["y"];
		} catch(...) {}
		try {
			p.XSpeed = (float)pack["XSpeed"];
		} catch(...) {}
		try {
			p.YSpeed = (float)pack["YSpeed"];
		} catch(...) {}
		try {
			p.padding3 = pack["padding3"];
		} catch(...) {}
		try {
			p.punchX = (int)pack["punchX"];
		} catch(...) {}
		try {
			p.punchY = (int)pack["punchY"];
		} catch(...) {}
		try {
			std::string data_tmp = pack["data"];
			data = std::move(data_tmp);
			p.setAdditionalData((uint8_t*)data.c_str(), data.length());
		} catch(...) {}
		auto peer = pack["peer"];
		if(peer.getType() == EventPackDataType::STRING) {
			p.send(server.getPeer(peer));
		} else {
			std::vector<std::string> peers = peer;
			for(auto x: peers) {
				p.send(server.getPeer(x));
			}
		}
	} catch(...) {
		std::cout << "Missing peer for TankPacket" << std::endl;
	}
}

EVENT(SendVariantPacket) {
	try {
		VariantPacket p;
		std::string packetName = pack["name"];
		p.appendString(packetName);
		try {
			for(int i=1; ; i++) {
				EventUnpackRequest item = pack[std::to_string(i)];
				switch(item.getType()) {
					case INTEGER: {
						p.appendIntx(item);
						break;
					}
					case FLOAT: {
						p.appendFloat(item);
						break;
					}
					case VECTOR_FLOAT: {
						std::vector<float> val = item;
						switch(val.size()) {
							case 1: {
								p.appendFloat(val[0]);
								break;
							}
							case 2: {
								p.appendFloat(val[0], val[1]);
								break;
							}
							case 3: {
								p.appendFloat(val[0], val[1], val[2]);
								break;
							}
							default: {
								std::cout << "Unsupported float vector size for VariantList " << packetName << "!" << std::endl;
							}
						}
						break;
					}
					case STRING: {
						p.appendString(item);
						break;
					}
					default: {
						std::cout << "Unsupported VarianList type for " << packetName << "!" << std::endl;
						break;
					}
				}
			}
		} catch(...) {

		}
		auto peer = pack["peer"];
		if(peer.getType() == EventPackDataType::STRING) {
			p.send(server.getPeer(pack["peer"]));
		} else {
			std::vector<std::string> peers = peer;
			for(auto x: peers) {
				p.send(server.getPeer(x));
			}
		}
	} catch(...) {
		std::cout << "Missing peer or packet name for VariantPacket" << std::endl;
	}
}

EVENT(SendDisconnect) {
	try {
		auto peer = pack["peer"];
		if(peer.getType() == EventPackDataType::STRING) {
			server.getPeer(peer).requestDisconnect();
		} else {
			std::vector<std::string> peers = peer;
			for(auto x: peers) {
				server.getPeer(x).requestDisconnect();
			}
		}
	} catch(...) {
		std::cout << "Missing peer for SendDisconnect" << std::endl;
	}
}

void registerServerEvents() {
	eventRegistrator.register_function("SendLoginRequestPacket", SendLoginRequestPacket);
	eventRegistrator.register_function("SendTankPacket", SendTankPacket);
	eventRegistrator.register_function("SendVariantPacket", SendVariantPacket);
	eventRegistrator.register_function("SendDisconnect", SendDisconnect);
}

void Server::OnConnect(Peer peer) {
	uint8_t* tmp = *events.currentDataPtr;
	std::string id = peer.getUID();
	{
		EventPack pack(X);
		pack["peer"] = peer.getUID();
		events.create_event_self("__MallocPeer", pack);
	}
	{
		EventPack pack(X);
		pack["peer"] = peer.getUID();
		events.create_event_self("OnPlayerConnect", pack);
	}
	*events.currentDataPtr = tmp;
}

void Server::OnPlayerActionPacket(Peer peer, std::string data) {
	EventPack pack(X);
	pack["peer"] = peer.getUID();
	pack["data"] = data;
	events.create_event_self("OnPlayerActionPacket", pack);
}

void Server::OnServerActionPacket(Peer peer, std::string data) {
	EventPack pack(X);
	pack["peer"] = peer.getUID();
	pack["data"] = data;
	events.create_event_self("OnServerActionPacket", pack);
}

void Server::OnTankPacket(Peer peer, TankPacketStruct data, uint8_t* additionalData, uint32_t additionalDataSize) {
	{
		EventPack pack(X);
		pack["peer"] = peer.getUID();
		pack["packetType"] = (int)data.packetType;
		pack["netID"] = (int)data.netID;
		pack["plantingTree"] = (int)data.plantingTree;
		pack["characterState"] = (int)data.characterState;
		pack["x"] = (float)data.x;
		pack["y"] = (float)data.y;
		pack["xSpeed"] = (float)data.XSpeed;
		pack["ySpeed"] = (float)data.YSpeed;
		pack["punchX"] = (int)data.punchX;
		pack["punchY"] = (int)data.punchY;
		std::string data;
		data.resize(additionalDataSize);
		memcpy((void*)data.c_str(), (void*)additionalData, additionalDataSize);
		pack["data"] = data;
		events.create_event_self("OnTankPacket", pack);
	}
}

void Server::OnUnknownPacket(Peer peer, int type) {
	EventPack pack(X);
	pack["text"] = "Please implement packet type: " + std::to_string(type);
	events.create_event_self("ConsoleLog", pack);
}

void Server::OnRecievedPacket(Peer peer, uint8_t* data, uint32_t dataLen) {
	uint8_t* tmp = *events.currentDataPtr;
	decode(peer, data, dataLen);
	*events.currentDataPtr = tmp;
}

void Server::OnDisconnect(Peer peer) {
	uint8_t* tmp = *events.currentDataPtr;
	EventPack pack(X);
	pack["peer"] = peer.getUID();
	events.create_event_self("OnPlayerDisconnect", pack);
	*events.currentDataPtr = tmp;
}

void Server::OnPeriodicEvent() {
	events.execute(*this);
}
