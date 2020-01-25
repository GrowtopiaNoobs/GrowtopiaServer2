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


#include "VariantList.hpp"

template <typename t, int exceptedSize>
void VariantList::appendValue(t val) {
	static_assert(sizeof(val) == exceptedSize, "Invalid VariantList data size (probably compiler issue)");
	this->data.insert(this->data.end(), sizeof(val), 0);
	memcpy(this->data.data() + this->data.size() - sizeof(val), &val, sizeof(val));
}

VariantList& VariantList::appendFloat(float val) {
	this->data.push_back(this->indexes);
	this->data.push_back(1);
	this->appendValue<float, 4>(val);
	this->indexes++;
	return *this;
}

VariantList& VariantList::appendFloat(float val, float val2)
{
	this->data.push_back(this->indexes);
	this->data.push_back(3);
	this->appendValue<float, 4>(val);
	this->appendValue<float, 4>(val2);
	this->indexes++;
	return *this;
}

VariantList& VariantList::appendFloat(float val, float val2, float val3)
{
	this->data.push_back(this->indexes);
	this->data.push_back(4);
	this->appendValue<float, 4>(val);
	this->appendValue<float, 4>(val2);
	this->appendValue<float, 4>(val3);
	this->indexes++;
	return *this;
}

VariantList& VariantList::appendInt(int val)
{
	this->data.push_back(this->indexes);
	this->data.push_back(9);
	this->appendValue<int, 4>(val);
	this->indexes++;
	return *this;
}

VariantList& VariantList::appendIntx(int val)
{
	this->data.push_back(this->indexes);
	this->data.push_back(5);
	this->appendValue<int, 4>(val);
	this->indexes++;
	return *this;
}

VariantList& VariantList::appendString(std::string str)
{
	this->data.push_back(this->indexes);
	this->data.push_back(2);
	int sLen = str.length();
	this->appendValue<int, 4>(sLen);
	for (int i = 0; i < sLen; i++) {
		this->data.push_back(str[i]);
	}
	this->indexes++;
	return *this;
}

std::vector<uint8_t> VariantList::getData() {
	std::vector<uint8_t> Tdata = this->data;
	Tdata.insert(Tdata.begin(), this->indexes);
	return Tdata;
}

VariantList::VariantList() {
	indexes = 0;
}

VariantList::VariantList(std::string packetName) {
	VariantList();
	this->appendString(packetName);
}

VariantList::~VariantList() {

}
