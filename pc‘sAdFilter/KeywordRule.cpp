#include "KeywordRule.h"

// 构造函数
KeywordRule::KeywordRule(const std::string& keyword) 
	: keyword_(keyword)
{
}

// 匹配逻辑：检查 url 是否包含 keyword_
bool KeywordRule::matches(const std::string& url, const std::string& host) const
{
	// 在 url 中查找 keyword_
	size_t pos = url.find(keyword_);
	
	
	if (pos == std::string::npos) 
	{
		return false;  
	} else 
	{
		return true;  
	}
}

// 返回规则类型
std::string KeywordRule::getType() const
{
	return "KEYWORD";
}