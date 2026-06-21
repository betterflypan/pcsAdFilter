#ifndef DOMAINRULE_H
#define DOMAINRULE_H

#include "IFilterRule.h"

class DomainRule : public IFilterRule
{
private:
	std::string domain_;  // 要拦截的域名

public:
	DomainRule(const std::string& domain);  
	
	// 实现基类的纯虚函数
	bool matches(const std::string& url, const std::string& host) const;
	std::string getType() const;
};

#endif