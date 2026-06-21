#ifndef KEYWORDRULE_H
#define KEYWORDRULE_H

#include "IFilterRule.h"

class KeywordRule : public IFilterRule
{
private:
	std::string keyword_;  // 要拦截的关键词

public:
	KeywordRule(const std::string& keyword);  // 构造函数
	
	// 实现基类的纯虚函数
	bool matches(const std::string& url, const std::string& host) const;
	std::string getType() const;
};

#endif