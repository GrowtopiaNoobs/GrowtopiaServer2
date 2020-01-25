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


#include <csignal>
#include <chrono>
#include <thread>
#include <iostream>

#include "ENetWrapper/ENetServer.hpp"
#include "Server.hpp"

void sigint(int) {
	std::cout << "Caught SIGINT signal! Stopping server..." << std::endl;
	enet_deinitialize();
	exit(0);
}

void sigterm(int) {
	std::cout << "Caught SIGTERM signal! Stopping server..." << std::endl;
	enet_deinitialize();
	exit(0);
}

// TODO: catch other signals too and execute stop event, before server is terminated

int main () {
	std::cout << "GrowServer v0.0.1 starting..." << std::endl;
	try {
		ENet_Init();
	} catch (...) {
		std::cout << "An error occurred while initializing ENet." << std::endl;
		return EXIT_FAILURE;
	}
	registerEventPools();
	modulesLoader.execute();
	registerServerEvents();
	std::vector<Server*> servers;
	for(int i=0; i<4; i++) {
		Server* s = new Server();
		servers.push_back(s);
	}
	int serverOffset = 0;
	const int serverBeginPort = 17091;
	for(Server* server: servers) {
		try {
		server->init(serverBeginPort + serverOffset, 128, "127.0.0.1");
		serverOffset++;
		} catch(...) {
			std::cout << "Error while initializing server " << serverOffset << ", port " << serverBeginPort + serverOffset << " is busy..." << std::endl;
			return 1;
		}
	}
	for(Server* server: servers) {
		server->execute();
	}
	signal(SIGINT, sigint);
	signal(SIGTERM, sigterm);
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}
