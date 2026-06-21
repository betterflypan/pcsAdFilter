#include "FilterEngine.h"

// 添加规则到链表(AI指导)
void FilterEngine::addRule(std::unique_ptr<IFilterRule> rule) 
{
	rules_.push_back(std::unique_ptr<IFilterRule>(rule.release()));
}

// 添加白名单规则：用于例外放行特定URL，避免误拦截正常内容
void FilterEngine::addWhiteList(const WhiteListRule& rule)
{
	whiteList_.push_back(rule);
}

// 核心：遍历所有规则，调用多态匹配
// 匹配优先级：白名单 > 黑名单 > 默认放行
// 输出参数 hitRule：返回匹配到的规则描述，用于外部日志记录
// 返回值：true=拦截 false=放行
bool FilterEngine::shouldBlock(const std::string& url, const std::string& host, std::string& hitRule) const 
{
	hitRule = "NONE";//默认无匹配
	
	// 第一步：检查白名单，匹配则直接放行，不再检查黑名单
	for (size_t i = 0; i < whiteList_.size(); ++i)
	{
		if (whiteList_[i].isMatch(url))
		{
			hitRule = whiteList_[i].getDescription();
			return false;//白名单命中，放行
		}
	}
	
	// 第二步：遍历黑名单规则链表
	for (size_t i = 0; i < rules_.size(); ++i) //size()是vector的函数，返回元素个数
	{
		// 调用虚函数 matches()，运行时动态绑定到实际规则类型
		if (rules_[i]->matches(url, host)) {
			// 命中：记录统计（按规则类型计数）
			stats_.recordHit(rules_[i]->getType());
			hitRule = rules_[i]->getType();//返回匹配到的规则类型，供外部日志使用
			return true;  // 拦截
		}
	}
	// 没有规则命中，放行
	return false;
}


// 内部调用三参数版本，忽略 hitRule 输出
bool FilterEngine::shouldBlock(const std::string& url, const std::string& host) const
{
	std::string dummy;
	return shouldBlock(url, host, dummy);
}

// 清空所有规则（包括黑名单和白名单）
// 不用考虑unique_ptr，因为unique_ptr自动释放内存
void FilterEngine::clearRules() 
{
	rules_.clear();
	whiteList_.clear();//同时清空白名单
}

// 获取当前黑名单规则数量
size_t FilterEngine::getRuleCount() const//size_t 是专门表示数量和大小的无符号整数类型
{
	return rules_.size();
}

// 获取白名单规则数量
size_t FilterEngine::getWhiteListCount() const
{
	return whiteList_.size();
}

// 获取统计对象引用，不复制，直接返回原对象
const RuleStatistics& FilterEngine::getStatistics() const 
{
	return stats_;
}