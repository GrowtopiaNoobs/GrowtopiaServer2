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


#ifndef __EVENTS_HANDLER_H__
#define __EVENTS_HANDLER_H__

#include "Event.h"
#include <list>
#include <unordered_map>
#include <chrono>
#include "../ENetWrapper/ENetServer.hpp"

struct EventRunnableStuff {
	std::vector<EventHandler> events;
	std::vector<GuardHandler> guards;
};

class EventRegistrator {
private:
	std::unordered_map<std::string, EventRunnableStuff> map;
public:
	void register_function(const std::string& key, EventHandler handler) {
		if(map.find(key) == map.end()) { // key not found
			EventRunnableStuff x;
			x.events.push_back(handler);
			map.insert({key, x});
		} else { // key found
			map.find(key)->second.events.push_back(handler);
		}
	}

	void register_guard(const std::string& key, GuardHandler handler) {
		auto item = map.find(key);
		if(item == map.end()) { // key not found
			EventRunnableStuff x;
			x.guards.push_back(handler);
			map.insert({key, x});
		} else { // key found
			item->second.guards.push_back(handler);
		}
	}

	void execute(const std::string& key, EventUnpack& pack, ENetServer& server, void* pool_ptr, uint8_t** currentDataPtr) {
		auto handlers = map.find(key);
		if(handlers == map.end()) return;
		bool canBeLaunched = true;
		char* serverID = (char*)server.getServerID().c_str();
		auto serverPtr = server.getServerPtr();
		uint8_t* tmp = *currentDataPtr;
		for(GuardHandler handler: handlers->second.guards) {
			try {
				bool x = (*handler)(pack.getData(), serverID, serverPtr, pool_ptr, currentDataPtr);
				if(!x) {
					canBeLaunched = false;
				}
			} catch(...) {
				std::cout << "Uncought exception in event-guard " << handlers->first << std::endl;
			}
		}
		*currentDataPtr = tmp;
		if(canBeLaunched) {
			for (EventHandler handler: handlers->second.events) {
				try {
					(*handler)(pack.getData(), serverID, serverPtr, pool_ptr, currentDataPtr);
				} catch (...) {
					std::cout << "Uncought exception in event " << handlers->first << std::endl;
				}
			}
		}
		*currentDataPtr = tmp;
	}
};

class FuncitonRegistrator {
private:
	std::unordered_map<std::string, std::vector<FucntionHandler>> map;
public:
	void register_function(std::string key, FucntionHandler handler) {
		if(map.find(key) == map.end()) { // key not found
			std::vector<FucntionHandler> vec;
			vec.push_back(handler);
			map.insert({key, vec});
		} else { // key found
			map.find(key)->second.push_back(handler);
		}
	}

	EventUnpack execute(const std::string& key, uint8_t* pack, char* serverName, void* serverPtr, void* pool_ptr, uint8_t** currentDataPtr) {
		uint8_t* ret;
		auto mapFuncs = map.find(key);
		if(mapFuncs == map.end()) return EventUnpack(NULL);
		for(FucntionHandler handler: mapFuncs->second) {
			ret = (*handler)(pack, serverName, serverPtr, pool_ptr, currentDataPtr);
			if(!EventUnpack(ret).isEmpty()) return EventUnpack(ret);
		}
		return EventUnpack(ret);
	}

	EventUnpack execute(const std::string& key, EventUnpack pack, ENetServer& server, void* pool_ptr, uint8_t** currentDataPtr) {
		uint8_t* ret;
		auto mapFuncs = map.find(key);
		if(mapFuncs == map.end()) return EventUnpack(NULL);
		for(FucntionHandler handler: mapFuncs->second) {
			ret = (*handler)(pack.getData(), (char *) server.getServerID().c_str(), server.getServerPtr(), pool_ptr, currentDataPtr);
			if(!EventUnpack(ret).isEmpty()) return EventUnpack(ret);
		}
		return EventUnpack(ret);
	}
};

extern EventRegistrator eventRegistrator;
extern FuncitonRegistrator functionRegistrator;

struct Event {
	std::string name;
	EventUnpack data;
	long long int time = 0;
public:
	Event(std::string name, EventUnpack data) : name(std::move(name)), data(data) {};
};

class EventsHandler {
	std::list<Event>& events;
public:
	void create_event(std::string name, EventUnpack data, long long int msTime=0) {
		int size = (*(int*)data.getData()) + 4;
		void* data_copy = malloc(size);
		memcpy(data_copy, data.getData(), size);
		Event event(std::move(name), (uint8_t*)data_copy);
		if(msTime!=0) {
			event.time = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch()).count() + msTime;
		} else {
			event.time = 0;
		}
		events.push_back(event);
	}

	bool operator==(std::list<Event>& x) {
		return addressof(x) == addressof(events);
	}

	explicit EventsHandler(std::list<Event> &events) : events(events) {}
};

class EventsHandlerPool {
	std::list<std::list<Event>> pool;
public:
	void execute(ENetServer& server, void* pool_ptr, uint8_t** currentDataPtr) {
		for(std::list<Event>& events: pool) {
			long long int time = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch()).count();
			auto i = events.begin();
			while (i != events.end()) {
				if (i->time == 0 || i->time <= time) {
					eventRegistrator.execute(i->name, i->data, server, pool_ptr, currentDataPtr);
					free(i->data.getData());
					events.erase(i++);
				} else {
					++i;
				}
			}
		}
	}

	EventsHandler requestHandler() {
		std::list<Event> v;
		pool.push_back(v);
		EventsHandler e(pool.back());
		return e;
	}

	bool isChild(EventsHandler x) {
		for(std::list<Event>& y: pool) {
			if (x==y) return true;
		}
		return false;
	}
};

extern std::list<EventsHandlerPool> pools;
extern std::list<std::vector<EventsHandler>> othersHandlerSyncer;

void registerEventPools();

class EventsGlobalPool {
private:
	EventsHandlerPool& pool;
	EventsHandler selfHandler;
	std::vector<EventsHandler>& othersHandler;
	uint8_t* temporaryData;

	static EventsHandlerPool& generateSelfHandler() {
		pools.emplace_back(EventsHandlerPool());
		return pools.back();
	}

	static std::vector<EventsHandler>& generateOthersHandlerSyncer() {
		othersHandlerSyncer.emplace_back(std::vector<EventsHandler>());
		return othersHandlerSyncer.back();
	}

public:
	uint8_t** currentDataPtr;
	void create_event_self(const std::string& name, EventUnpack data, long long int msTime=0) {
		selfHandler.create_event(std::move(name), data, msTime);
	}

	void create_event_global(const std::string& name, const EventUnpack& data, long long int msTime=0) {
		for(EventsHandler handler: othersHandler) {
			handler.create_event(name, data, msTime);
		}
	}

	void create_event_self_and_global(const std::string& name, const EventUnpack& data, long long int msTime=0) {
		create_event_self(name, data, msTime);
		create_event_global(name, data, msTime);
	}

	EventUnpack call_function(const std::string& key, EventUnpack pack, ENetServer& server) {
		return functionRegistrator.execute(key, pack, server, (void*)this, currentDataPtr);
	}

	void execute(ENetServer& server) {
		pool.execute(server, (void*)this, currentDataPtr);
	}

	EventsGlobalPool() : pool(generateSelfHandler()), selfHandler(pool.requestHandler()), othersHandler(generateOthersHandlerSyncer()) {
		for(EventsHandlerPool& p: pools) {
			if(!p.isChild(selfHandler)) {
				othersHandler.emplace_back(p.requestHandler());
			}
		}
		for(std::vector<EventsHandler>& x: othersHandlerSyncer) {
			if(addressof(x)!=addressof(othersHandler)) {
				x.emplace_back(pool.requestHandler());
			}
		}
		temporaryData = new uint8_t[16000000];
		currentDataPtr = new uint8_t*;
		*currentDataPtr = temporaryData;
	}
};

#include <iostream>
#include <dirent.h>
#include <dlfcn.h>

namespace ModulesLoaderUtils {
	bool hasEnding(std::string const &fullString, std::string const &ending);
}

#define MODULE_API_VERSION 1

void moduleRegisterEvent(char* name, void* event);
void moduleRegisterEventGuard(char* name, void* event);
void moduleRegisterFunction(char* name, void* event);

void module_create_event_self(void* ptr, char* name, uint8_t* data, long long int msTime);
void module_create_event_global(void* ptr, char* name, uint8_t* data, long long int msTime);
void module_create_event_self_and_global(void* ptr, char* name, uint8_t* data, long long int msTime);
uint8_t* module_call_function(void* eventGeneratorPtr, char* name, uint8_t* data, char* serverName, void* serverPtr, uint8_t** memPtr);

#ifdef _WIN32 
#include <windows.h>
#endif

class ModulesLoader {
public:
	void execute() {
#ifdef _WIN32 
		HANDLE hFind;
		WIN32_FIND_DATA data;

		hFind = FindFirstFile("./modules/*", &data);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (ModulesLoaderUtils::hasEnding(data.cFileName, ".dll")) {
					std::string totalPath = "./modules/";
					totalPath += data.cFileName;
					HINSTANCE handle = LoadLibrary(totalPath);
					if(handle == NULL) {
						std::cout << "DLL open error: " << GetLastError() << std::endl;
						continue;
					}
					int (*moduleVerFunction)() = (int (*)()) GetProcAddress(handle, "ModuleGetVersion");
					if (moduleVerFunction == NULL) {
						std::cout << "Dynamic library " << data.cFileName << " isn't valid server module!" << std::endl;
						continue;
					}
					int moduleVer = moduleVerFunction();
					if (moduleVer == MODULE_API_VERSION) {
						std::cout << "Loading module " << data.cFileName << "..." << std::endl;
						((void (*)(void *, void *, void *, void *, void *, void*, void*)) GetProcAddress(handle, "ModuleExecuteSetup"))((void*)moduleRegisterEvent, (void*)moduleRegisterEventGuard, (void*)moduleRegisterFunction, (void*)module_create_event_self, (void*)module_create_event_global, (void*)module_create_event_self_and_global, (void*)module_call_function);
					} else {
						std::cout << "Module " << data.cFileName << " (" << moduleVer <<") have different API version than server (" << MODULE_API_VERSION << ")." << std::endl;
					}
				}
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
			std::cout << "All modules were loaded!" << std::endl;
		} else {
			std::cout << "Error opening ./modules/ directory" << std::endl;
		}
#else
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir("./modules/")) != NULL) {
			/* print all the files and directories within directory */
			while ((ent = readdir(dir)) != NULL) {
				if (ModulesLoaderUtils::hasEnding(ent->d_name, ".so")) {
					std::string totalPath = "./modules/";
					totalPath += ent->d_name;
					void *handle = dlopen(totalPath.c_str(), RTLD_LAZY);
					if(handle == NULL) {
						std::cout << dlerror() << std::endl;
						continue;
					}
					int (*moduleVerFunction)() = (int (*)()) dlsym(handle, "ModuleGetVersion");
					if (moduleVerFunction == NULL) {
						std::cout << "Dynamic library " << ent->d_name << " isn't valid server module!" << std::endl;
						continue;
					}
					int moduleVer = moduleVerFunction();
					if (moduleVer == MODULE_API_VERSION) {
						std::cout << "Loading module " << ent->d_name << "..." << std::endl;
						((void (*)(void *, void *, void *, void *, void *, void*, void*)) dlsym(handle, "ModuleExecuteSetup"))((void*)moduleRegisterEvent, (void*)moduleRegisterEventGuard, (void*)moduleRegisterFunction, (void*)module_create_event_self, (void*)module_create_event_global, (void*)module_create_event_self_and_global, (void*)module_call_function);
					} else {
						std::cout << "Module " << ent->d_name << " (" << moduleVer <<") have different API version than server (" << MODULE_API_VERSION << ")." << std::endl;
					}

				}
			}
			closedir(dir);
			std::cout << "All modules were loaded!" << std::endl;
		} else {
			std::cout << "Error opening ./modules/ directory" << std::endl;
		}
#endif
	}
};

extern ModulesLoader modulesLoader;
#endif
