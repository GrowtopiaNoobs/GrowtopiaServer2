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
#include <list>
#include <vector>
#include <cstring>
#include <memory>

enum EventPackDataType : short {
	INTEGER_T,
	VECTOR_INTEGER_T,
	LONG_INTEGER_T,
	VECTOR_LONG_INTEGER_T,
	FLOAT_T,
	VECTOR_FLOAT_T,
	STRING_T,
	VECTOR_STRING_T,
	EVENT_PACK_T,
	VECTOR_EVENT_PACK_T
};

class EventPack;

class EventPackData {
private:
	std::string name;
	std::vector<__uint8_t> data;

	template <typename T>
	void push_back(T item, unsigned int *pushed_size = NULL) {
		if(pushed_size) *pushed_size += sizeof(item);
		for(int i=0; i<sizeof(item); i++) {
			data.push_back(*(((__uint8_t*)&item)+i));
		}
	}

	template <typename T>
	void push_back(T *item, unsigned int size) {
		for(int i=0; i<size; i++) {
			data.push_back(*(((__uint8_t*)item)+i));
		}
	}

	int push_header(EventPackDataType type) {
		short nameLen = this->name.length();
		this->push_back(nameLen);
		this->push_back(name.c_str(), nameLen);
		this->push_back(type);
		int ret = data.size();
		unsigned int size = 0;
		this->push_back(size);
		return ret;
	}

	inline void add_data_size(unsigned int pos, unsigned int data_size) {
		((unsigned int*)(this->data.data()+pos))[0] += data_size;
	}
public:
	void operator=(int arg);
	void operator=(std::vector<int> arg);
	void operator=(long long int arg);
	void operator=(std::vector<long long int> arg);
	void operator=(float arg);
	void operator=(std::vector<float> arg);
	void operator=(std::string arg);
	void operator=(std::vector<std::string> arg);
	void operator=(EventPack arg);
	void operator=(std::vector<EventPack> arg);

	std::vector<__uint8_t> getData() {
		return data;
	}

	EventPackData(std::string name) {
		this->name = name;
	}
};

class EventPack {
private:
	std::list<EventPackData> data;
	uint8_t** ptr;

public:
	bool isEmpty() {
		if(data.size()==0) return true;
		return false;
	}

	__uint8_t* serialize(int* size = NULL) {
		int s = 0;
		for(EventPackData& pack: data) {
			s += pack.getData().size();
		}
		if(size) *size = s + sizeof(int);
		__uint8_t* ret = *ptr;
		*ptr += s + sizeof(int);
		__uint8_t* ptr = ret + sizeof(int);
		memcpy(ret, &s, sizeof(int));
		for(EventPackData& pack: data) {
			int dataSize = pack.getData().size();
			std::memcpy(ptr, pack.getData().data(), dataSize);
			ptr += dataSize;
		}
		return ret;
	}

	EventPackData& operator[](std::string name) {
		EventPackData dat(std::move(name));
		data.emplace_back(dat);
		return data.back();
	}

	EventPack(uint8_t** ptr) {
		this->ptr = ptr;
	}
};

class EventUnpack;

class EventUnpackRequest {
private:
	std::string parName;
	uint8_t* data;

	uint8_t* getPtrByName(std::string name) {
		if(data == NULL) return NULL;
		int nameLen = name.length();
		uint8_t* data_ptr = data;
		int length = *(int*)data_ptr;
		for(uint8_t* ptr = data_ptr+4; ptr<data_ptr+length;) {
			unsigned short name2Len = *(unsigned short*)ptr;
			ptr += sizeof(unsigned short);
			if(name2Len!=nameLen) {
				ptr+=name2Len;
				ptr+=sizeof(EventPackDataType);
				ptr+=*(int*)ptr;
				ptr+=sizeof(int);
				continue;
			}
			for(int i=0; i<nameLen; i++) {
				if(ptr[i]!=name[i]) {
					ptr+=name2Len;
					ptr+=sizeof(EventPackDataType);
					ptr+=*(int*)ptr;
					ptr+=sizeof(int);
					goto ptr_search_continue;
				}
			}
			ptr+=name2Len;
			return ptr;
			ptr_search_continue:;
		}
		return NULL;
	}

	static_assert(sizeof(EventPackDataType) == sizeof(short), "Please fix following functions, when size does not match!");

public:
	static void resolve_value(uint8_t* ptr, int& ret) {
		if(*(short*)ptr!=EventPackDataType::INTEGER_T) throw;
		ptr+=sizeof(short)+sizeof(int);
		ret = *(int*)ptr;
	}

	static void resolve_value(uint8_t* ptr, std::vector<int>& ret) {
		if(*(short*)ptr!=EventPackDataType::VECTOR_INTEGER_T) throw;
		ptr+=sizeof(short);
		unsigned int var_count = (*(unsigned int*)ptr) >> 2;
		ptr+=sizeof(int);
		for(int i=0; i<var_count; i++) {
			ret.push_back(*(int*)ptr);
			ptr+=sizeof(int);
		}
	}

	static void resolve_value(uint8_t* ptr, unsigned int& ret) {
		if(*(short*)ptr!=EventPackDataType::INTEGER_T) throw;
		ptr+=sizeof(short)+sizeof(int);
		ret = *(int*)ptr;
	}

	static void resolve_value(uint8_t* ptr, std::vector<unsigned int>& ret) {
		if(*(short*)ptr!=EventPackDataType::VECTOR_INTEGER_T) throw;
		ptr+=sizeof(short);
		unsigned int var_count = (*(unsigned int*)ptr) >> 2;
		ptr+=sizeof(int);
		for(int i=0; i<var_count; i++) {
			ret.push_back(*(int*)ptr);
			ptr+=sizeof(int);
		}
	}

	static void resolve_value(uint8_t* ptr, long long int& ret) {
		if(*(short*)ptr!=EventPackDataType::LONG_INTEGER_T) throw;
		ptr+=sizeof(short)+sizeof(int);
		ret = *(long long int*)ptr;
	}

	static void resolve_value(uint8_t* ptr, std::vector<long long int>& ret) {
		if(*(short*)ptr!=EventPackDataType::VECTOR_LONG_INTEGER_T) throw;
		ptr+=sizeof(short);
		unsigned int var_count = (*(unsigned int*)ptr) >> 3;
		ptr+=sizeof(int);
		for(int i=0; i<var_count; i++) {
			ret.push_back(*(long long int*)ptr);
			ptr+=sizeof(long long int);
		}
	}

	static void resolve_value(uint8_t* ptr, float& ret) {
		if(*(short*)ptr!=EventPackDataType::FLOAT_T) throw;
		ptr+=sizeof(short)+sizeof(int);
		ret = *(float*)ptr;
	}

	static void resolve_value(uint8_t* ptr, std::vector<float>& ret) {
		if(*(short*)ptr!=EventPackDataType::VECTOR_FLOAT_T) throw;
		ptr+=sizeof(short);
		unsigned int var_count = (*(unsigned int*)ptr) >> 2;
		ptr+=sizeof(int);
		for(int i=0; i<var_count; i++) {
			ret.push_back(*(float*)ptr);
			ptr+=sizeof(float);
		}
	}

	static void resolve_value(uint8_t* ptr, std::string& ret) {
		if(*(short*)ptr!=EventPackDataType::STRING_T) throw;
		ptr+=sizeof(short);
		int strLen = *(int*)ptr;
		ptr+=sizeof(int);
		ret.resize(strLen);
		for(int i=0; i<strLen; i++) {
			ret[i]=*(ptr++);
		}
	}

	static void resolve_value(uint8_t* ptr, std::vector<std::string>& ret) {
		if(*(short*)ptr!=EventPackDataType::VECTOR_STRING_T) throw;
		ptr+=sizeof(short);
		int data_size = *(int *) ptr;
		ptr += sizeof(int);
		for(int i=0; i<data_size; ) {
			int strLen = *(int *) ptr;
			ptr += sizeof(int);
			data_size += sizeof(int);
			std::string str;
			str.resize(strLen);
			for (int i = 0; i < strLen; i++) {
				ret[i] = *(ptr++);
			}
			data_size += strLen;
		}
	}

	static void resolve_value(uint8_t* ptr, EventUnpack& ret);
	static void resolve_value(uint8_t* ptr, std::vector<EventUnpack>& ret);

	template <typename T>
	operator T() {
		uint8_t* ptr = getPtrByName(parName);
		if(ptr==NULL) throw 1;
		T ret;
		resolve_value(ptr, ret);
		return ret;
	}

	EventPackDataType getType() {
		uint8_t* ptr = getPtrByName(parName);
		if(ptr==NULL) throw 1;
		return *(EventPackDataType*)ptr;
	}

	EventUnpackRequest(std::string parName, uint8_t* data) : parName(std::move(parName)), data(std::move(data)) {}
};

#include <sstream>

class EventUnpack {
private:
	uint8_t* data;



public:
	EventUnpackRequest operator[](std::string name) {
		return EventUnpackRequest(name, data);
	}

	uint8_t* getData() {
		return data;
	}

	bool isEmpty() {
		if(data == NULL) return true;
		if(*(int*)data>4) return true;
		return false;
	}

	std::string to_string() {
		if(data == NULL) {
			return "NULL EventPack";
		}
		std::string ret = "";
		uint8_t* data_ptr = data;
		int length = *(int*)data_ptr;
		for(uint8_t* ptr = data_ptr+4; ptr<data_ptr+length;) {
			unsigned short nameLen = *(unsigned short*)ptr;
			ptr += sizeof(unsigned short);
			std::string name = "";
			for(int i=0; i<nameLen; i++) {
				name += ptr[i];
			}
			ptr+=nameLen;
			EventPackDataType type = *(EventPackDataType*)ptr;
			uint32_t dataSize = *(int*)(ptr + sizeof(EventPackDataType));
			std::string res2 = name + " (type: ";
			switch(type) {
				case EventPackDataType::INTEGER_T: res2 += "INTEGER_T"; break;
				case EventPackDataType::VECTOR_INTEGER_T: res2 += "VECTOR_INTEGER_T"; break;
				case EventPackDataType::LONG_INTEGER_T: res2 += "LONG_INTEGER_T"; break;
				case EventPackDataType::VECTOR_LONG_INTEGER_T: res2 += "VECTOR_LONG_INTEGER_T"; break;
				case EventPackDataType::FLOAT_T: res2 += "FLOAT_T"; break;
				case EventPackDataType::VECTOR_FLOAT_T: res2 += "VECTOR_FLOAT_T"; break;
				case EventPackDataType::STRING_T: res2 += "STRING_T"; break;
				case EventPackDataType::VECTOR_STRING_T: res2 += "VECTOR_STRING_T"; break;
				case EventPackDataType::EVENT_PACK_T: res2 += "EVENT_PACK_T"; break;
				case EventPackDataType::VECTOR_EVENT_PACK_T: res2 += "VECTOR_EVENT_PACK_T"; break;
				default: res2 += "UNKNOWN"; break;
			}
			res2 += ", size: " + std::to_string(dataSize) + " bytes):\n";
			std::string res = "";
			switch(type) {
				case EventPackDataType::INTEGER_T: {
					int val;
					EventUnpackRequest::resolve_value(ptr, val);
					res += std::to_string(val) + "\n";
					break;
				}
				case EventPackDataType::VECTOR_INTEGER_T: {
					std::vector<int> val;
					EventUnpackRequest::resolve_value(ptr, val);
					for(int i=0; i<val.size(); i++) {
						res += "[" + std::to_string(i) + "]: " + std::to_string(val[i]) + "\n";
					}
					break;
				}
				case EventPackDataType::LONG_INTEGER_T: {
					long long int val;
					EventUnpackRequest::resolve_value(ptr, val);
					res += std::to_string(val) + "\n";
					break;
				}
				case EventPackDataType::VECTOR_LONG_INTEGER_T: {
					std::vector<long long int> val;
					EventUnpackRequest::resolve_value(ptr, val);
					for(int i=0; i<val.size(); i++) {
						res += "[" + std::to_string(i) + "]: " + std::to_string(val[i]) + "\n";
					}
					break;
				}
				case EventPackDataType::FLOAT_T: {
					float val;
					EventUnpackRequest::resolve_value(ptr, val);
					res += std::to_string(val) + "\n";
					break;
				}
				case EventPackDataType::VECTOR_FLOAT_T: {
					std::vector<float> val;
					EventUnpackRequest::resolve_value(ptr, val);
					for(int i=0; i<val.size(); i++) {
						res += "[" + std::to_string(i) + "]: " + std::to_string(val[i]) + "\n";
					}
					break;
				}
				case EventPackDataType::STRING_T: {
					std::string val;
					EventUnpackRequest::resolve_value(ptr, val);
					res += val + "\n";
					break;
				}
				case EventPackDataType::VECTOR_STRING_T: {
					std::vector<std::string> val;
					EventUnpackRequest::resolve_value(ptr, val);
					for(int i=0; i<val.size(); i++) {
						res += "[" + std::to_string(i) + "]: " + val[i] + "\n";
					}
					break;
				}
				case EventPackDataType::EVENT_PACK_T: {
					EventUnpack val(NULL);
					EventUnpackRequest::resolve_value(ptr, val);
					res += val.to_string() + "\n";
					break;
				}
				case EventPackDataType::VECTOR_EVENT_PACK_T: {
					std::vector<EventUnpack> val;
					EventUnpackRequest::resolve_value(ptr, val);
					for(int i=0; i<val.size(); i++) {
						res += "[" + std::to_string(i) + "]: " + val[i].to_string() + "\n";
					}
					break;
				}
				default: {
					res += "UNKNOWN";
					break;
				}
			}
			std::istringstream f(res);
			std::string line;
			bool isFirst = true;
			while (std::getline(f, line)) {
				if(isFirst) {
					res2 += " |  \n";
					res2 += " |-- " + line + "\n";
					isFirst = false;
				} else {
					res2 += "	 " + line + "\n";
				}
			}
			ret += res2;
			ptr+=sizeof(EventPackDataType);
			ptr+=*(int*)ptr;
			ptr+=sizeof(int);
		}
		std::string retf = "EventPack:\n";
		std::istringstream f(ret);
		std::string line;
		while (std::getline(f, line)) {
			retf += "  " + line + "\n";
		}
		return retf;
	}

	operator std::string() {
		return to_string();
	}

	EventUnpack(uint8_t* data) : data(data) {}
	EventUnpack(EventPack pack) : data(pack.serialize(NULL)) {}
};

#include "../ENetWrapper/ENetServerRemote.hpp"

typedef void (*EventHandler)(uint8_t *pack, char* serverName, void* serverPtr, void* current_event_caller, uint8_t** currentDataPtr);
typedef char (*GuardHandler)(uint8_t* pack, char* serverName, void* serverPtr, void* current_event_caller, uint8_t** currentDataPtr);
typedef uint8_t* (*FucntionHandler)(uint8_t* pack, char* serverName, void* serverPtr, void* current_event_caller, uint8_t** currentDataPtr);

#define EVENT(fname)	void fname ## _WRAPPER(EventUnpack pack, ENetServerRemote server, void* __current_event_caller, uint8_t** __currentDataPtr); \
			void fname(uint8_t* data, char* serverName, void* serverPtr, void* current_event_caller, uint8_t** __currentDataPtr) {\
				fname ## _WRAPPER(EventUnpack(data), ENetServerRemote(serverName, (ENetHost*)serverPtr), current_event_caller, __currentDataPtr); \
			} \
			void fname ## _WRAPPER(EventUnpack pack, ENetServerRemote server, void* __current_event_caller, uint8_t** __currentDataPtr)

#define FUNCTION(fname)	EventUnpack fname ## _WRAPPER(EventUnpack pack, ENetServerRemote server, void* __current_event_caller, uint8_t** __currentDataPtr); \
			uint8_t* fname(uint8_t* data, char* serverName, void* serverPtr, void* current_event_caller, uint8_t** __currentDataPtr) {\
				return fname ## _WRAPPER(EventUnpack(data), ENetServerRemote(serverName, (ENetHost*)serverPtr), current_event_caller, __currentDataPtr).getData(); \
			} \
			EventUnpack fname ## _WRAPPER(EventUnpack pack, ENetServerRemote server, void* __current_event_caller, uint8_t** __currentDataPtr)
