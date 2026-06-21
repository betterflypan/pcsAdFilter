#ifndef RULESTATISTICS_H
#define RULESTATISTICS_H

#include <map>
#include <string>

class RuleStatistics 
{
private:
	std::map<std::string, int> hitCount_;  // 记录每种规则的命中次数
	//标准库容器类型+模板参数：键是字符串，值是整数

public:
	// 记录一次命中
	void recordHit(const std::string& ruleType);
	
	// 查询某类规则的命中次数
	int getHitCount(const std::string& ruleType) const;
	
	// 打印所有统计信息
	void printAll() const;
};

#endif