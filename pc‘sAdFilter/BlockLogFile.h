#ifndef BLOCK_LOG_FILE_H
#define BLOCK_LOG_FILE_H

#include <string>
#include <fstream>
#include <windows.h>  // CRITICAL_SECTION, SYSTEMTIME

// 日志记录类型
enum LogType {
    BLOCKED = 0,    // 被拦截
    ALLOWED,        // 正常放行
    ERROR_TYPE      // 程序错误（避免和宏ERROR冲突）
};

class BlockLogFile {
public:
    // 构造函数：指定日志文件路径
    BlockLogFile(const std::string& filename = "adfilter.log");
    
    // 析构函数：关闭文件
    ~BlockLogFile();

    // 记录一条日志（线程安全）
    void log(LogType type, const std::string& rule, const std::string& url);

    // 获取统计信息
    int getBlockedCount() const { return blockedCount_; }
    int getAllowedCount() const { return allowedCount_; }
    int getErrorCount() const { return errorCount_; }
    int getTotalCount() const { return blockedCount_ + allowedCount_ + errorCount_; }

    // 格式化输出统计信息
    std::string getStatsString() const;

private:
    std::ofstream file_;           // 文件流
    CRITICAL_SECTION cs_;          // Windows 临界区（替代 std::mutex）
    
    int blockedCount_;             // 拦截计数
    int allowedCount_;             // 放行计数
    int errorCount_;               // 错误计数

    // 获取当前时间字符串
    std::string getCurrentTime() const;
    
    // 将LogType转为字符串
    std::string typeToString(LogType type) const;
};

#endif // BLOCK_LOG_FILE_H