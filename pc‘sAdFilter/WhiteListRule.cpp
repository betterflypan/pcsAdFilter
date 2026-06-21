#include "WhiteListRule.h"

WhiteListRule::WhiteListRule(const std::string& pattern)
	: pattern_(pattern)
{
}

bool WhiteListRule::isMatch(const std::string& url) const
{
	// 子串匹配：URL 包含白名单模式即放行
	return url.find(pattern_) != std::string::npos;
}

std::string WhiteListRule::getDescription() const
{
	return "WHITELIST " + pattern_;
}