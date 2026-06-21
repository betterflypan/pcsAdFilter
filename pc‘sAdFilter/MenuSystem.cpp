#include "MenuSystem.h"
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <conio.h>

void MenuSystem::drawDoubleLineBorder(int width) {
    std::cout << "  " << (char)201;
    for (int i = 0; i < width - 2; i++) std::cout << (char)205;
    std::cout << (char)187 << std::endl;
}

void MenuSystem::drawSingleLineBorder(int width) {
    std::cout << "  " << (char)218;
    for (int i = 0; i < width - 2; i++) std::cout << (char)196;
    std::cout << (char)191 << std::endl;
}

void MenuSystem::drawSeparator(int width) {
    std::cout << "  " << (char)204;
    for (int i = 0; i < width - 2; i++) std::cout << (char)205;
    std::cout << (char)185 << std::endl;
}

int MenuSystem::getIntInput(int min, int max, const std::string& prompt) {
    int value;
    while (true) {
        std::cout << "  " << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        std::cout << "  [错误] 输入无效，请输入 " << min << "-" << max << " 之间的数字。" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }
}

std::string MenuSystem::getStringInput(const std::string& prompt, int maxLength) {
    std::cout << "  " << prompt;
    std::string input;
    std::getline(std::cin, input);
    if ((int)input.length() > maxLength) {
        input = input.substr(0, maxLength);
    }
    return input;
}

void MenuSystem::showProgressBar(const std::string& label, int percent) {
    std::cout << "  " << label << " [";
    int filled = percent / 5;
    for (int i = 0; i < 20; i++) {
        if (i < filled) std::cout << (char)219;
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << percent << "%" << std::endl;
}

void MenuSystem::showLoading(const std::string& msg, int durationMs) {
    const char* anim = "|/-\\";
    int steps = durationMs / 100;
    for (int i = 0; i < steps; i++) {
        std::cout << "\r  " << msg << " " << anim[i % 4] << " ";
        Sleep(100);
    }
    std::cout << "\r  " << msg << " 完成！" << std::endl;
}