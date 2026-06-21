#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include <winsock2.h>
#include <process.h>  // _beginthreadex
#include "FilterEngine.h"
#include "BlockLogFile.h"

class ProxyServer
{
public:
	ProxyServer(int port, FilterEngine& engine, BlockLogFile& logger);
	~ProxyServer();
	
	bool start();   // 创建线程启动代理，立即返回不阻塞
	void stop();    // 停止线程，清理资源
	bool isRunning() const;
	void handleClient(int clientSocket);
	// 静态线程函数：Windows线程入口必须是全局或静态
	static unsigned int __stdcall threadFunc(void* param);

private:
	int port_;
	FilterEngine& engine_;
	BlockLogFile& logger_;
	SOCKET listenSocket_;
	volatile bool running_;   // volatile：确保多线程可见性
	HANDLE threadHandle_;     // 线程句柄，用于等待线程结束
	
	void runLoop();           // 实际的代理循环（在线程中运行）
	
};

#endif