#include "DomainRule.h"

// 构造函数：初始化成员变量 domain_
DomainRule::DomainRule(const std::string& domain) 
	: domain_(domain)  
{
}

// 匹配逻辑：检查 host 是否包含 domain_
bool DomainRule::matches(const std::string& url, const std::string& host) const
{
	// 第一步：调用 find()，返回位置
	size_t pos = host.find(domain_);
	
	// 第二步：判断是不是 npos
	if (pos == std::string::npos) 
	{
		return false;
	} else 
	{
		return true;
	}
}

// 返回规则类型
std::string DomainRule::getType() const
{
	return "DOMAIN";
}