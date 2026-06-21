#include <iostream>
#include <winsock2.h>
#include "FilterEngine.h"
#include "ConfigManager.h"
#include "ProxyServer.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	FilterEngine engine;

	if (!ConfigManager::loadFromFile("config.txt", engine))
	{
		std::cerr << "Failed to load config.txt" << std::endl;
		WSACleanup();
		return 1;
	}

	std::cout << "Loaded " << engine.getRuleCount() << " rules." << std::endl;

	// 启动代理服务器（关键区别）
	ProxyServer server(8080, engine);
	if (server.start())
	{
		std::cout << "Proxy server started on port 8080..." << std::endl;
	}

	WSACleanup();
	return 0;
}