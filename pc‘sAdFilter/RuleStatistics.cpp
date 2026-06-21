#include "RuleStatistics.h"
#include <iostream>

// 记录命中：map中对应类型计数+1
void RuleStatistics::recordHit(const std::string& ruleType) 
{
	hitCount_[ruleType]++;
}

// 查询某类规则的命中次数
int RuleStatistics::getHitCount(const std::string& ruleType) const 
{
	// VS2010用iterator遍历map
	std::map<std::string, int>::const_iterator it = hitCount_.find(ruleType);
	if (it != hitCount_.end()) 
	{
		return it->second;  // 找到，返回计数
	}
	return 0;  // 没找到，返回0
}

// 打印所有统计
void RuleStatistics::printAll() const 
{
	std::cout << "========== Rule Statistics ==========" << std::endl;
	
	// VS2010用iterator遍历map
	for (std::map<std::string, int>::const_iterator it = hitCount_.begin();
		 it != hitCount_.end(); ++it) 
	{
		std::cout << "  " << it->first << ": " << it->second << " hits" << std::endl;
	}
	
	std::cout << "=====================================" << std::endl;
}