#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <conio.h>

ConsoleUI::ConsoleUI()
{
}

ConsoleUI::~ConsoleUI()
{
	setColor(7);
}

void ConsoleUI::init()
{
	// 设置控制台输出代码页为 GBK
	SetConsoleOutputCP(936);
	
	// 设置控制台标题
	SetConsoleTitleA("AdFilter 广告过滤系统 v1.0");
	
	// 设置窗口大小
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD bufferSize = {100, 500};
	SetConsoleScreenBufferSize(hOut, bufferSize);
	
	// 设置窗口尺寸
	SMALL_RECT windowSize = {0, 0, 99, 29};
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);
	
	// 隐藏光标
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cursorInfo);
}

void ConsoleUI::setColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void ConsoleUI::clearScreen()
{
	system("cls");
}

void ConsoleUI::pause()
{
	std::cout << "\n  按任意键继续...";
	_getch();
}

void ConsoleUI::drawBorder(int width, char topLeft, char topRight,
						   char bottomLeft, char bottomRight,
						   char horizontal, char vertical)
{
	std::cout << "  " << topLeft;
	for (int i = 0; i < width - 2; i++)
	{
		std::cout << horizontal;
	}
	std::cout << topRight << std::endl;
}

void ConsoleUI::printCentered(const std::string& text, int width)
{
	int padding = (width - 2 - (int)text.length()) / 2;
	std::cout << "  " << "|";
	for (int i = 0; i < padding; i++)
	{
		std::cout << " ";
	}
	std::cout << text;
	for (int i = 0; i < width - 2 - padding - (int)text.length(); i++)
	{
		std::cout << " ";
	}
	std::cout << "|" << std::endl;
}

void ConsoleUI::printMenuItem(int number, const std::string& text, bool isHighlight)
{
	if (isHighlight)
	{
		setColor(14 | 16);
	}
	else
	{
		setColor(7);
	}
	
	std::cout << "  " << "|" << "  ";
	std::cout << "[" << number << "] " << std::left << std::setw(20) << text;
	
	if (isHighlight)
	{
		std::cout << "  <-- 当前选中";
	}
	std::cout << std::right;
	
	int used = 4 + 3 + 22 + (isHighlight ? 14 : 0);
	for (int i = used; i < 50; i++)
	{
		std::cout << " ";
	}
	std::cout << "|" << std::endl;
	
	setColor(7);
}

void ConsoleUI::showBanner()
{
	setColor(11);
	std::cout << std::endl;
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	printCentered("AdFilter 广告过滤系统", 60);
	printCentered("本地HTTP代理 | 轻量高效 | 浏览器无关", 60);
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	std::cout << std::endl;
	setColor(7);
}

void ConsoleUI::showProxyStatus(bool running, int port)
{
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	
	std::cout << "  " << "|" << "  代理状态: ";
	if (running)
	{
		setColor(10);
		std::cout << "运行中";
		setColor(7);
		std::cout << "  端口: " << port;
	}
	else
	{
		setColor(12);
		std::cout << "已停止";
		setColor(7);
		std::cout << "  端口: --";
	}
	
	// 补齐右边空格
	std::cout << "                                            ";
	std::cout << "|" << std::endl;
	
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	std::cout << std::endl;
}

int ConsoleUI::showMainMenu()
{
	showBanner();
	
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	printCentered("主 菜 单", 60);
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	
	printMenuItem(1, "启动代理服务");
	printMenuItem(2, "停止代理服务");
	printMenuItem(3, "查看拦截统计");
	printMenuItem(4, "查看最近日志");
	printMenuItem(5, "重载过滤规则");
	printMenuItem(6, "清屏");
	printMenuItem(7, "退出程序");
	printMenuItem(8, "启动/停止测试服务器");
	
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	
	std::cout << "\n  请选择操作 [1-8]: ";
	int choice;
	std::cin >> choice;
	std::cin.clear();
	std::cin.ignore(10000, '\n');
	
	return choice;
}

void ConsoleUI::showStatistics(int blocked, int allowed, int total)
{
	clearScreen();
	showBanner();
	
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	printCentered("拦 截 统 计", 60);
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	
	setColor(10);
	std::cout << "  " << "|" << "  正常放行: " << std::setw(8) << allowed << " 次";
	std::cout << "                            " << "|" << std::endl;
	
	setColor(12);
	std::cout << "  " << "|" << "  广告拦截: " << std::setw(8) << blocked << " 次";
	std::cout << "                            " << "|" << std::endl;
	
	setColor(7);
	std::cout << "  " << "|" << "  总请求数: " << std::setw(8) << total << " 次";
	std::cout << "                            " << "|" << std::endl;
	
	setColor(7);
	std::cout << "  ";
	drawBorder(60, '+', '+', '+', '+', '-', '|');
	
	pause();
}

void ConsoleUI::showLogPreview(const std::vector<std::string>& recentLogs)
{
	clearScreen();
	showBanner();
	
	std::cout << "  ";
	drawBorder(80, '+', '+', '+', '+', '-', '|');
	printCentered("最 近 日 志 (最近20条)", 80);
	std::cout << "  ";
	drawBorder(80, '+', '+', '+', '+', '-', '|');
	
	for (size_t i = 0; i < recentLogs.size() && i < 20; i++)
	{
		std::string line = recentLogs[i];
		if (line.find("BLOCKED") != std::string::npos)
		{
			setColor(12);
		}
		else if (line.find("ALLOWED") != std::string::npos)
		{
			setColor(10);
		}
		else
		{
			setColor(7);
		}
		
		if (line.length() > 74)
		{
			line = line.substr(0, 71) + "...";
		}
		
		std::cout << "  " << "|" << " " << std::left << std::setw(76) << line
				  << "|" << std::endl;
		setColor(7);
	}
	
	std::cout << "  ";
	drawBorder(80, '+', '+', '+', '+', '-', '|');
	
	pause();
}

void ConsoleUI::showConfigReloadResult(bool success, int ruleCount)
{
	if (success)
	{
		setColor(10);
		std::cout << "\n  [成功] 规则重载完成！共加载 " << ruleCount << " 条规则。" << std::endl;
	}
	else
	{
		setColor(12);
		std::cout << "\n  [失败] 规则重载失败！请检查 config.txt 文件。" << std::endl;
	}
	setColor(7);
	pause();
}

void ConsoleUI::showMessage(const std::string& msg, bool isError)
{
	if (isError)
	{
		setColor(12);
		std::cout << "\n  [错误] " << msg << std::endl;
	}
	else
	{
		setColor(10);
		std::cout << "\n  [提示] " << msg << std::endl;
	}
	setColor(7);
	pause();
}