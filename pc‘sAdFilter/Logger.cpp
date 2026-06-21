#include "Logger.h"
#include <iostream>

// 记录拦截：输出到控制台
void Logger::logBlock(const std::string& url, const std::string& ruleType) {
	std::cout << "[BLOCKED] Rule=" << ruleType 
	          << " | URL=" << url << std::endl;
}

// 记录普通信息
void Logger::logInfo(const std::string& msg) {
	std::cout << "[INFO] " << msg << std::endl;
}

// 记录错误信息（输出到cerr，通常显示为红色）
void Logger::logError(const std::string& msg) {
	std::cerr << "[ERROR] " << msg << std::endl;
}

// 打印统计报表
void Logger::printStatistics(int blockedCount, int totalCount) {
	std::cout << "========== Statistics ==========" << std::endl;
	std::cout << "Blocked: " << blockedCount << std::endl;
	std::cout << "Total: " << totalCount << std::endl;
	
	// 计算拦截率，更加直观，防止除零
	double rate = (totalCount > 0) 
		? (100.0 * blockedCount / totalCount) 
		: 0.0;
	
	std::cout << "Block rate: " << rate << "%" << std::endl;
	std::cout << "==============================" << std::endl;
}