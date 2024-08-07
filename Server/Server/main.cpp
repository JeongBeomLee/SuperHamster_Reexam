#include "GameServer.h"
#include <iostream>
#include <string>

int main()
{
	GameServer server;
	server.Start(PORT);

	while (true) {
		std::string command;
		std::cin >> command;

		if (command == "exit")
			break;
	}

	server.Stop();
}