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

#include <string>
#include <cstring>
#include <enet/enet.h>
#include <vector>

class VariantList
{
private:
	std::vector<uint8_t> data;
	int indexes = 0;

	template <typename t, int e>
	void appendValue(t val);
public:
	VariantList& appendFloat(float val);
	VariantList& appendFloat(float val1, float val2);
	VariantList& appendFloat(float val1, float val2, float val3);
	VariantList& appendInt(int val);
	VariantList& appendIntx(int val);
	VariantList& appendString(std::string str);
	std::vector<uint8_t> getData();

	VariantList();
	VariantList(std::string packetName);
	~VariantList();
};
