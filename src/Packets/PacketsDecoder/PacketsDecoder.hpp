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

#include <iostream>
#include "../PacketsCommon/PacketsCommon.hpp"
#include "../../ENetWrapper/Peer.hpp"

class PacketDecoder {
private:
	std::string dataToString(uint8_t* data, uint32_t dataLen) {
		std::string ret;
		ret.resize(dataLen-1);
		std::memcpy((void*)ret.data(), data, dataLen - 1);
		return ret;
	}

	uint8_t* dataToTankData(uint8_t* data, uint32_t len) {
		uint8_t* ret = NULL;
		if (len >= 56)
		{
			TankPacketStruct* tankData = (TankPacketStruct*)data;
			if (tankData->characterState & 8)
			{
				if (len < tankData->packetLenght + 56)
				{
					ret = 0;
				}
			}
			else {
				tankData->packetLenght = 0;
			}
		}
		return ret;
	}

protected:
	virtual void OnPlayerActionPacket(Peer peer, std::string data);
	virtual void OnServerActionPacket(Peer peer, std::string data);
	virtual void OnTankPacket(Peer peer, TankPacketStruct data, uint8_t* additionalData, uint32_t additionalDataSize);
	virtual void OnUnknownPacket(Peer peer, int type);

public:
	void decode(Peer peer, uint8_t* data, uint32_t length) {
		if (length < 5) throw;
		int packetType = *(int*)data;
		data += 4;
		length -= 4;
		switch (packetType) {
		case 2:
		{
			OnPlayerActionPacket(peer, dataToString(data, length));
			break;
		}
		case 3:
		{
			OnServerActionPacket(peer, dataToString(data, length));
			break;
		}
		case 4:
		{
			if (length < 56) throw;
			uint8_t* tankUpdatePacket = dataToTankData(data, length);
			TankPacketStruct tankData = *(TankPacketStruct*)data;
			if (tankUpdatePacket) {
				
				uint8_t* dataPtr;
				if (length > sizeof(TankPacketStruct)) {
					dataPtr = tankUpdatePacket + sizeof(TankPacketStruct);
				} else {
					dataPtr = NULL;
				}
				OnTankPacket(peer, tankData, dataPtr, length - sizeof(TankPacketStruct));
			} else {
				OnTankPacket(peer, tankData, 0, 0);
			}
			break;
		}
		/*case 5:
		{

			break;
		}
		case 6:
		{
			std::cout << "Got packet type 6 with content: " << PacketHandler::GetTextPointerFromPacket(packet)) << std::endl;
			break;
		}*/
		default:
		{
			OnUnknownPacket(peer, packetType);
			break;
		}
		}
	}
};

inline void PacketDecoder::OnPlayerActionPacket(Peer peer, std::string data)
{
	std::cout << "OnPlayerActionPacket is not set! forgot to override?" << std::endl;
}

inline void PacketDecoder::OnServerActionPacket(Peer peer, std::string data)
{
	std::cout << "OnServerActionPacket is not set! forgot to override?" << std::endl;
}

inline void PacketDecoder::OnTankPacket(Peer peer, TankPacketStruct data, uint8_t* additionalData,
	uint32_t additionalDataSize)
{
	std::cout << "OnTankPacket is not set! forgot to override?" << std::endl;
}

inline void PacketDecoder::OnUnknownPacket(Peer peer, int type)
{
	std::cout << "OnUnknownPacket is not set! forgot to override?" << std::endl;
}
