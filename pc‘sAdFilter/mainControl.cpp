#include <iostream>
#include <winsock2.h>
#include "FilterEngine.h"
#include "ConfigManager.h"
#include "ProxyServer.h"
#include "BlockLogFile.h"
#include "ConsoleUI.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
	// 初始化Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	// 创建核心对象
	FilterEngine engine;
	BlockLogFile logger;
	ProxyServer server(8080, engine, logger);
	ConsoleUI ui;
	TestServer testServer(8081);
	
	// 加载配置文件
	if (!ConfigManager::loadFromFile("config.txt", engine))
	{
		ui.showMessage("配置文件加载失败！请检查 config.txt", true);
		return 1;
	}
	
	// 初始化控制台界面
	ui.init();
	
	bool running = false;
	int choice;
	
	do
	{
		ui.clearScreen();
		ui.showProxyStatus(running, 8080);
		choice = ui.showMainMenu();
		
		switch (choice)
		{
		case 1: // 启动代理服务
			if (!running)
			{
				if (server.start())
				{
					running = true;
					ui.showMessage("代理服务已启动！端口 8080\n请设置浏览器代理为 127.0.0.1:8080");
				}
				else
				{
					ui.showMessage("启动失败！端口被占用", true);
				}
			}
			else
			{
				ui.showMessage("代理已在运行中！");
			}
			break;
			
		case 2: // 停止代理服务
			if (running)
			{
				server.stop();
				running = false;
				ui.showMessage("代理服务已停止！\n请关闭浏览器代理设置");
			}
			else
			{
				ui.showMessage("代理未运行！", true);
			}
			break;
			
		case 3: // 查看拦截统计
			{
				const RuleStatistics& stats = engine.getStatistics();
				int total = logger.getTotalCount();
				ui.showStatistics(logger.getBlockedCount(), logger.getAllowedCount(), 
								total);
			}
			break;
			
		case 4: // 查看最近日志
			{
				std::vector<std::string> logs;
    
				// 打开日志文件（文件名必须和 BlockLogFile 创建时的一致）
				// 默认是 "adfilter.log"
				std::ifstream logFile("adfilter.log");
    
				if (logFile.is_open())
				{
					std::string line;
					// 一行一行读，全部存进 logs
					while (std::getline(logFile, line))
					{
						if (!line.empty())  // 跳过空行
							logs.push_back(line);
					}
					logFile.close();
        
					// 文件可能很长（几百条），但菜单只显示得下 20 条（用户友好）
					// 所以只保留最后 20 行
					if (logs.size() > 20)
					{
						// 把 logs 变成"从倒数第20行开始到末尾"这一段
						logs = std::vector<std::string>(logs.end() - 20, logs.end());
					}
				}
    
				// 把读到的日志传给菜单显示
				ui.showLogPreview(logs);
			}
			break;
			
		case 5: // 重载过滤规则
			engine.clearRules();
			if (ConfigManager::loadFromFile("config.txt", engine))
			{
				ui.showConfigReloadResult(true, engine.getRuleCount());
			}
			else
			{
				ui.showConfigReloadResult(false, 0);
			}
			break;
			
		case 6: // 清屏（用户友好)
			ui.clearScreen();
			break;
			
		case 8: // 启动/停止测试服务器
			if (!testServer.isRunning())
			{
				if (testServer.start())
				{
					ui.showMessage("测试服务器已启动！端口 8081\n"
								   "测试地址：http://localhost:8081/normal.html\n"
								   "广告地址：http://localhost:8081/ad.html\n"
								   "提示：需同时启动代理(菜单1)并设浏览器代理为127.0.0.1:8080，\n"
								   "访问 ad.html 才能看到拦截效果。");
				}
				else
				{
					ui.showMessage("测试服务器启动失败！端口 8081 被占用", true);
				}
			}
			else
			{
				testServer.stop();
				ui.showMessage("测试服务器已停止！");
			}
			break;

		case 7: // 退出程序
			if (running)
			{
				server.stop();
				running = false;
			}
			break;
			
		default:
			ui.showMessage("无效选择，请重新输入！", true);
			break;
		}

		
	} while (choice != 7);
	
	// 清理Winsock
	WSACleanup();
	return 0;
}