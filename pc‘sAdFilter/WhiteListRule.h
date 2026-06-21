#ifndef WHITE_LIST_RULE_H
#define WHITE_LIST_RULE_H

#include <string>

// 白名单规则：优先放行，不匹配则继续检查其他规则
class WhiteListRule {
public:
    explicit WhiteListRule(const std::string& pattern);
    
    // 检查 URL 是否在白名单中
    bool isMatch(const std::string& url) const;
    
    // 获取规则描述
    std::string getDescription() const;

private:
    std::string pattern_;  // 白名单模式（子串匹配）
};

#endif // WHITE_LIST_RULE_H