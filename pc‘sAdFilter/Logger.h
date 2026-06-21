#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
	// 记录拦截信息
	static void logBlock(const std::string& url, const std::string& ruleType);
	
	// 记录普通信息
	static void logInfo(const std::string& msg);
	
	// 记录错误信息
	static void logError(const std::string& msg);
	
	// 打印统计报表
	static void printStatistics(int blockedCount, int totalCount);
};

#endif