#ifndef IFILTERRULE_H
#define IFILTERRULE_H
//头文件保护宏：防止同一个头文件被重复包含，避免编译错误
//代码使用std::string而尽量不使用using namespace std;调用string
 //因为头文件会被多个源文件包含，强行注入命名空间会引发难以追踪的冲突

#include <string>

class IFilterRule 
{
	public:
		virtual bool matches(const std::string& url, const std::string& host) const = 0;
		virtual std::string getType() const = 0;
		virtual ~IFilterRule() {}
		//析构函数必须是虚的，否则派生类析构会被跳过，资源无法完全释放
};

#endif