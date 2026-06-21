#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <string>
#include <vector>
#include "TestServer.h"

// 菜单选项
enum MenuOption {
    MENU_START_PROXY = 1,   // 启动代理
    MENU_STOP_PROXY,        // 停止代理
    MENU_SHOW_STATS,        // 查看统计
    MENU_VIEW_LOG,          // 查看日志
    MENU_RELOAD_CONFIG,     // 重载规则
    MENU_CLEAR_SCREEN,      // 清屏
    MENU_EXIT               // 退出程序
};

class ConsoleUI {
public:
    ConsoleUI();
    ~ConsoleUI();

    // 初始化控制台（设置颜色、大小、标题）
    void init();

    // 显示主菜单并获取用户选择
    int showMainMenu();

    // 显示各种界面
    void showBanner();              // 顶部横幅
    void showProxyStatus(bool running, int port);  // 代理状态
    void showStatistics(int blocked, int allowed, int total);  // 统计信息
    void showLogPreview(const std::vector<std::string>& recentLogs);  // 日志预览
    void showConfigReloadResult(bool success, int ruleCount);  // 重载结果
    void showMessage(const std::string& msg, bool isError = false);  // 提示消息

    // 清屏
    void clearScreen();

    // 等待用户按任意键
    void pause();

    // 设置控制台颜色
    void setColor(int color);

private:
    // 绘制边框
    void drawBorder(int width, char topLeft, char topRight, char bottomLeft, char bottomRight, 
                    char horizontal, char vertical);
    
    // 居中打印文字
    void printCentered(const std::string& text, int width);
    
    // 打印带边框的菜单项
    void printMenuItem(int number, const std::string& text, bool isHighlight = false);
};

#endif // CONSOLE_UI_H