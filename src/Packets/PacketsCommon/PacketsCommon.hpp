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

struct TankPacketStruct {
	uint32_t packetType = 0;
	int32_t netID = 0;
	uint32_t padding1 = 0;
	uint32_t characterState = 0;
	uint32_t padding2 = 0;
	uint32_t plantingTree = 0;
	float x = 0;
	float y = 0;
	float XSpeed = 0;
	float YSpeed = 0;
	uint32_t padding3 = 0;
	uint32_t punchX = 0;
	uint32_t punchY = 0;
	uint32_t packetLenght = 0;
};

static_assert(sizeof(TankPacketStruct) == 56, "Ivalid size of TankPacketStruct, maybe your compiler isnt supported");
