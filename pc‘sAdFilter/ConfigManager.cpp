#include "ConfigManager.h"
#include "RuleFactory.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool ConfigManager::loadFromFile(const std::string& filename, FilterEngine& engine) 
{
	// 打开文件
	std::ifstream infile(filename.c_str());
	//std::ifstream 的构造函数只接受 const char*，不接受 std::string,故需要转换
	if (!infile) {
		std::cout << "Cannot open " << filename << std::endl;
		return false;
	}
	
	std::string line;
	
	// 逐行读取
	while (std::getline(infile, line)) 
	{
		// 跳过空行
		if (line.empty()) continue;
		
		// 跳过注释行（以 # 开头）
		if (line[0] == '#') continue;
		
		// 解析类型和参数
		std::istringstream iss(line);
		//line对iss初始化，同时istringstream
		std::string type, param;
		iss >> type >> param;

		// 白名单规则：单独处理，不经过工厂创建（白名单不是 IFilterRule 类型）
		if (type == "WHITELIST")
		{
			engine.addWhiteList(WhiteListRule(param));//直接添加白名单，不走工厂
			continue;//处理完毕，继续下一行
		}
		
		// 调用工厂创建规则RuleFactory（DOMAIN、KEYWORD等黑名单规则）
		std::unique_ptr<IFilterRule> rule = RuleFactory::createRule(type, param);
		if (rule)  // 检查是否创建成功（非空指针）
		{
			engine.addRule(std::unique_ptr<IFilterRule>(rule.release()));
		}
	}
	return true;
}