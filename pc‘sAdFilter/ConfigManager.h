#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include "FilterEngine.h"

class ConfigManager {
public:
	// 从文件加载规则，注入到引擎
	static bool loadFromFile(const std::string& filename, FilterEngine& engine);
};

#endif