#include "RuleFactory.h"
#include "DomainRule.h"
#include "KeywordRule.h"

std::unique_ptr<IFilterRule> RuleFactory::createRule(const std::string& type, const std::string& param)
{
	if (type == "DOMAIN")
	{
		return std::unique_ptr<IFilterRule>(new DomainRule(param));
		//new 一个 DomainRule 对象，传入参数 param，包装成 unique_ptr<IFilterRule> 返回
	}
	else if (type == "KEYWORD")
	{
		return std::unique_ptr<IFilterRule>(new KeywordRule(param));
	}
	
	// 后续扩展：REGEX、ALLOW 等类型在这里加
	// else if (type == "REGEX") { return ... }
	
	return std::unique_ptr<IFilterRule>();  // 空指针，表示创建失败
}