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

#include "../PacketsCommon/PacketsCommon.hpp"
#include "VariantList/VariantList.hpp"

#include "../../ENetWrapper/ENetServer.hpp"


class Packet {
public:
	virtual void send(Peer peer) {
		std::cout << "Please implement send method for Packet" << std::endl;
	}
};

class TankPacket : public TankPacketStruct, public Packet {
private:
	uint8_t* data = NULL;
	int dataLenght = 0;
public:
	void setAdditionalData(uint8_t* data, int dataLenght) {
		this->data = data;
		this->dataLenght = dataLenght;

	}

	void send(Peer peer) override {
		if (data == NULL || dataLenght == 0) {
			TankPacketStruct data = *this;
			this->packetLenght = 0;
			peer.sendPacket(4, (char*)&data, sizeof(TankPacketStruct));
		} else {
			int sizeOfDataToSend = sizeof(TankPacketStruct) + dataLenght;
			this->packetLenght = dataLenght;
			uint8_t* dataToSend = new uint8_t[sizeOfDataToSend];
			TankPacketStruct dataStruct = *this;
			memcpy(dataToSend, &dataStruct, sizeof(TankPacketStruct));
			memcpy(dataToSend + sizeof(TankPacketStruct), this->data, dataLenght);
			peer.sendPacket(4, (char*)dataToSend, sizeOfDataToSend);
			delete dataToSend;
		}
	}
};

class VariantPacket : public VariantList, public TankPacket {
private:
	void setAdditionalData(uint8_t* data, int dataLenght);
public:
	VariantPacket() : VariantList() {
		packetType = 1;
		netID = -1;
		characterState = 8;
	}

	VariantPacket(std::string name, int netID = -1) : VariantList(name) {
		VariantPacket();
		this->netID = netID;
		packetType = 1;
		characterState = 8;
	}

	void send(Peer peer) override {
		std::vector<uint8_t> tempData = getData();
		TankPacket::setAdditionalData(tempData.data(), tempData.size());
		TankPacket::send(peer);
	}
};

class TextPacket : public Packet {
private:
	std::string data;
public:
	TextPacket(std::string text = "") {
		this->data = text;
	}

	void setText(std::string text) {
		this->data = text;
	}

	void send(Peer peer) override {
		peer.sendPacket(3, (char*)data.c_str(), data.length());
	}
};

class LoginRequestPacket : public Packet {
public:
	void send(Peer peer) override {
		peer.sendPacket(1, 0, 0);
	}
};
