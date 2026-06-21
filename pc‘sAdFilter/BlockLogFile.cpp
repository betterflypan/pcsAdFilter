#include "BlockLogFile.h"
#include <iostream>
#include <sstream>
#include <iomanip>

BlockLogFile::BlockLogFile(const std::string& filename)
	: blockedCount_(0), allowedCount_(0), errorCount_(0)
{
	InitializeCriticalSection(&cs_);

	file_.open(filename.c_str(), std::ios::out | std::ios::app);//追加模式

	if (!file_.is_open())
	{
		// 只在初始化失败时输出错误信息（程序启动前，不会打断菜单）
		std::cerr << "[ERROR] 无法打开日志文件: " << filename << std::endl;
	}
	else
	{
		// 写入启动分隔线（文件读写功能）
		file_ << "\n========================================" << std::endl;
		file_ << "  AdFilter 启动时间: " << getCurrentTime() << std::endl;
		file_ << "========================================" << std::endl;
		file_.flush();
	}
}

BlockLogFile::~BlockLogFile()
{
	if (file_.is_open())
	{
		// 写入关闭信息和统计总结（文件读写功能）
		file_ << "========================================" << std::endl;
		file_ << "  AdFilter 关闭时间: " << getCurrentTime() << std::endl;
		file_ << "  总计: 拦截=" << blockedCount_
			  << " 放行=" << allowedCount_
			  << " 错误=" << errorCount_ << std::endl;
		file_ << "========================================" << std::endl;
		file_.close();
	}

	DeleteCriticalSection(&cs_);
}

// 记录日志：线程安全地写入文件（不输出控制台，避免打断菜单）
void BlockLogFile::log(LogType type, const std::string& rule, const std::string& url)
{
	EnterCriticalSection(&cs_);

	// 更新计数器
	switch (type)
	{
		case BLOCKED:    blockedCount_++; break;
		case ALLOWED:    allowedCount_++; break;
		case ERROR_TYPE: errorCount_++;   break;
	}

	// 文件读写：写入一行日志到 adfilter.log
	if (file_.is_open())
	{
		file_ << "[" << getCurrentTime() << "] "
			  << "[" << typeToString(type) << "] "
			  << "Rule=" << rule << " | "
			  << "URL=" << url << std::endl;
		file_.flush();
	}

	// 不输出控制台，避免打断菜单界面
	// 日志通过菜单 [3] 查看统计 或 直接打开 adfilter.log 文件查看

	LeaveCriticalSection(&cs_);
}

// 格式化统计信息字符串（用于菜单显示）
std::string BlockLogFile::getStatsString() const
{
	std::ostringstream oss;//ostringstream 可以方便地把数字和字符串混合拼接
	int total = getTotalCount();
	

	oss << "统计: 拦截=" << blockedCount_
		<< " 放行=" << allowedCount_
		<< " 错误=" << errorCount_
		<< " 总计=" << total;
	return oss.str();
}

// 获取当前时间字符串
std::string BlockLogFile::getCurrentTime() const
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	char buf[64];
	sprintf_s(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);

	return std::string(buf);
}

// LogType 转字符串
std::string BlockLogFile::typeToString(LogType type) const
{
	switch (type)
	{
		case BLOCKED:    return "BLOCKED";
		case ALLOWED:    return "ALLOWED";
		case ERROR_TYPE: return "ERROR";
		default:         return "UNKNOWN";
	}
}