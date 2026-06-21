#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <string>

class MenuSystem {
public:
    // 绘制双线边框
    static void drawDoubleLineBorder(int width);
    
    // 绘制单线边框
    static void drawSingleLineBorder(int width);
    
    // 绘制分隔线
    static void drawSeparator(int width);
    
    // 安全获取整数输入（带范围验证）
    static int getIntInput(int min, int max, const std::string& prompt);
    
    // 安全获取字符串输入
    static std::string getStringInput(const std::string& prompt, int maxLength);
    
    // 显示进度条
    static void showProgressBar(const std::string& label, int percent);
    
    // 显示加载动画
    static void showLoading(const std::string& msg, int durationMs);
};

#endif // MENU_SYSTEM_H