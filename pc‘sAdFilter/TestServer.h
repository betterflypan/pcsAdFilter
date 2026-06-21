#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <winsock2.h>
#include <string>

// 本地测试服务器：用于无网络环境下的答辩演示
// 提供正常网页和广告页面，验证代理拦截功能
class TestServer
{
public:
    TestServer(int port = 8081);
    ~TestServer();

    bool start();      // 启动测试服务器
    void stop();       // 停止测试服务器
    bool isRunning() const;

private:
    int port_;                 // 监听端口（默认8081）
    SOCKET listenSocket_;      // 监听socket
    bool running_;             // 运行标志
    HANDLE threadHandle_;      // 工作线程句柄

    static unsigned int __stdcall threadFunc(void* param);
    void runLoop();            // 主循环：接受连接
    void handleClient(int clientSocket); // 处理浏览器请求
    void sendResponse(int clientSocket, int code, const std::string& contentType, const std::string& body);
};

#endif