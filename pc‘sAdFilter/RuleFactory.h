#ifndef RULEFACTORY_H
#define RULEFACTORY_H

#include "IFilterRule.h"
#include <string>
#include <memory>

class RuleFactory
{
public:
	static std::unique_ptr<IFilterRule> createRule(const std::string& type, const std::string& param);
};

#endif