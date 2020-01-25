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


#include "EventsHandler.h"

EventRegistrator eventRegistrator;
FuncitonRegistrator functionRegistrator;
std::list<EventsHandlerPool> pools;
std::list<std::vector<EventsHandler>> othersHandlerSyncer;

void registerEventPools() {
	pools = std::list<EventsHandlerPool>();
	othersHandlerSyncer = std::list<std::vector<EventsHandler>>();
}

bool ModulesLoaderUtils::hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

void moduleRegisterEvent(char* name, void* event) {
	std::string nameS = name;
	eventRegistrator.register_function(name, (EventHandler)event);
}

void moduleRegisterEventGuard(char* name, void* event) {
	std::string nameS = name;
	eventRegistrator.register_guard(name, (GuardHandler)event);
}

void moduleRegisterFunction(char* name, void* event) {
	std::string nameS = name;
	functionRegistrator.register_function(name, (FucntionHandler)event);
}

void module_create_event_self(void* ptr, char* name, uint8_t* data, long long int msTime) {
	EventsGlobalPool* pool = (EventsGlobalPool*)ptr;
	pool->create_event_self(name, data, msTime);
}

void module_create_event_global(void* ptr, char* name, uint8_t* data, long long int msTime) {
	EventsGlobalPool* pool = (EventsGlobalPool*)ptr;
	pool->create_event_global(name, data, msTime);
}

void module_create_event_self_and_global(void* ptr, char* name, uint8_t* data, long long int msTime) {
	EventsGlobalPool* pool = (EventsGlobalPool*)ptr;
	pool->create_event_self_and_global(name, data, msTime);
}

uint8_t* module_call_function(void* eventGeneratorPtr, char* name, uint8_t* data, char* serverName, void* serverPtr, uint8_t** memPtr) {
	return functionRegistrator.execute(name, data, serverName, serverPtr, eventGeneratorPtr, memPtr).getData();
}

ModulesLoader modulesLoader;
