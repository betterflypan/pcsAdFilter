#ifndef FILTERENGINE_H
#define FILTERENGINE_H

#include <vector>//动态数组，存规则对象
#include <memory>
#include <string>
#include "IFilterRule.h"
#include "RuleStatistics.h"
#include "WhiteListRule.h"//白名单规则：优先放行，避免误拦截

class FilterEngine 
{
private:
	std::vector<std::unique_ptr<IFilterRule> > rules_;  // 规则链表,其实就是动态数组,用法rules_[i]
	std::vector<WhiteListRule> whiteList_;//白名单列表：匹配则直接放行，不进入黑名单检查
	mutable RuleStatistics stats_;  // mutable对象可变：即使对象被声明为 const，这个成员仍可修改

public:
	// 添加规则到链表(AI指导)
	void addRule(std::unique_ptr<IFilterRule> rule);
	
	// 添加白名单规则：用于例外放行特定URL，避免误拦截正常内容
	void addWhiteList(const WhiteListRule& rule);
	
	// 核心：遍历所有规则，调用多态匹配
	// 匹配优先级：白名单 > 黑名单 > 默认放行
	// 输出参数 hitRule：返回匹配到的规则描述，用于外部日志记录
	// 返回值：true=拦截 false=放行
	bool shouldBlock(const std::string& url, const std::string& host, std::string& hitRule) const;
	
	// 重载：不带输出参数的版本（兼容旧代码）
	bool shouldBlock(const std::string& url, const std::string& host) const;
	
	// 清空所有规则（包括黑名单和白名单）
	// 不用考虑unique_ptr，因为unique_ptr自动释放内存
	void clearRules();
	
	// 获取当前黑名单规则数量
	size_t getRuleCount() const;//size_t 是专门表示数量和大小的无符号整数类型
	
	// 获取白名单规则数量
	size_t getWhiteListCount() const;
	
	// 获取统计对象引用，不复制，直接返回原对象
	const RuleStatistics& getStatistics() const;
};

#endif