#include <iostream>
#include <winsock2.h>
#include "FilterEngine.h"
#include "ConfigManager.h"

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

	// ±¾µØ²âÊÔÆ¥Åä
	std::cout << "\n=== ¹æÔòÆ¥Åä²âÊÔ ===" << std::endl;
	
	std::string testUrl1 = "http://www.example.com/index.html";
	std::string testHost1 = "www.example.com";
	std::cout << "Test1 (Õý³£ÍøÒ³): " 
	          << (engine.shouldBlock(testUrl1, testHost1) ? "BLOCKED" : "ALLOWED") 
	          << std::endl;

	std::string testUrl2 = "http://doubleclick.net/ads/banner.jpg";
	std::string testHost2 = "doubleclick.net";
	std::cout << "Test2 (ÓòÃûºÚÃûµ¥): " 
	          << (engine.shouldBlock(testUrl2, testHost2) ? "BLOCKED" : "ALLOWED") 
	          << std::endl;

	std::string testUrl3 = "http://example.com/ads/popup.js";
	std::string testHost3 = "example.com";
	std::cout << "Test3 (¹Ø¼ü´ÊÆ¥Åä): " 
	          << (engine.shouldBlock(testUrl3, testHost3) ? "BLOCKED" : "ALLOWED") 
	          << std::endl;

	WSACleanup();
	std::cout << "\nPress Enter to exit...";
	std::cin.get();
	return 0;
}