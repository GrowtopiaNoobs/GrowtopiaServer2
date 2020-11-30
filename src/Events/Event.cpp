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


#include "Event.h"


void EventPackData::operator=(int arg) {
	int sizePos = push_header(EventPackDataType::INTEGER_T);
	this->push_back(arg);
	add_data_size(sizePos, sizeof(int));
}

void EventPackData::operator=(std::vector<int> arg) {
	int sizePos = push_header(EventPackDataType::VECTOR_INTEGER_T);
	for(int val: arg) {
		this->push_back(val);
	}
	add_data_size(sizePos, sizeof(int)*arg.size());
}


void EventPackData::operator=(long long int arg) {
	int sizePos = push_header(EventPackDataType::LONG_INTEGER_T);
	this->push_back(arg);
	add_data_size(sizePos, sizeof(long long int));
}

void EventPackData::operator=(std::vector<long long int> arg) {
	int sizePos = push_header(EventPackDataType::VECTOR_LONG_INTEGER_T);
	for(long long int val: arg) {
		this->push_back(val);
	}
	add_data_size(sizePos, sizeof(long long int)*arg.size());
}

void EventPackData::operator=(float arg) {
	int sizePos = push_header(EventPackDataType::FLOAT_T);
	this->push_back(arg);
	add_data_size(sizePos, sizeof(int));
}

void EventPackData::operator=(std::vector<float> arg) {
	int sizePos = push_header(EventPackDataType::VECTOR_FLOAT_T);
	for(int val: arg) {
		this->push_back(val);
	}
	add_data_size(sizePos, sizeof(int)*arg.size());
}

void EventPackData::operator=(std::string arg) {
	int sizePos = push_header(EventPackDataType::STRING_T);
	this->push_back(arg.c_str(), arg.length());
	add_data_size(sizePos, arg.length());
}

void EventPackData::operator=(std::vector<std::string> arg) {
	int sizePos = push_header(EventPackDataType::VECTOR_STRING_T);
	int length_final = 0;
	for(std::string val: arg) {
		int len = val.length();
		this->push_back(len);
		this->push_back(val.c_str(), len);
		length_final += len + sizeof(int);
	}
	add_data_size(sizePos, length_final);
}

void EventPackData::operator=(EventPack arg) {
	int sizePos = push_header(EventPackDataType::EVENT_PACK_T);
	int dataSize;
	uint8_t *data = arg.serialize(&dataSize);
	this->push_back(data, dataSize);
	add_data_size(sizePos, dataSize);
}

void EventPackData::operator=(std::vector<EventPack> arg) {
	int sizePos = push_header(EventPackDataType::VECTOR_EVENT_PACK_T);
	int length_final = 0;
	for(EventPack& pack: arg) {
		int dataSize;
		uint8_t *data = pack.serialize(&dataSize);
		this->push_back(dataSize);
		this->push_back(data, dataSize);
		length_final += dataSize + sizeof(int);
	}
	add_data_size(sizePos, length_final);
}

void EventPackData::operator=(EventUnpack arg) {
	int sizePos = push_header(EventPackDataType::EVENT_PACK_T);
	int dataSize;
	if(arg.getData() == NULL) {
		dataSize = 0;
	} else {
		dataSize =  *(int*)arg.getData();
	}
	uint8_t *data = arg.getData();
	this->push_back(data, dataSize+sizeof(int));
	add_data_size(sizePos, dataSize+sizeof(int));
}

void EventUnpackRequest::resolve_value(uint8_t* ptr, EventUnpack& ret) {
	if(*(short*)ptr!=EventPackDataType::EVENT_PACK_T) throw;
	ptr+=sizeof(short);
	if(*(int*)ptr == 0) {
		ret = EventUnpack(NULL);
	} else {
		ptr+=sizeof(int);
		ret = EventUnpack(ptr);
	}
}

void EventUnpackRequest::resolve_value(uint8_t* ptr, std::vector<EventUnpack>& ret) {
	if(*(short*)ptr!=EventPackDataType::VECTOR_EVENT_PACK_T) throw;
	ptr+=sizeof(short);
	int size=*(int*)ptr;
	ptr+=sizeof(int);
	for(int i=0; i<size; ) {
		int pack_size = *(int*)ptr;
		ptr+=sizeof(int);
		i+=sizeof(int);
		ret.push_back(EventUnpack(ptr));
		ptr+=pack_size;
		i+=pack_size;
	}
}
