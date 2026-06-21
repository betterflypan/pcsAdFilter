#include "ProxyServer.h"
#include <iostream>
#include <string>

// ========== 调试日志：运行时会输出详细流程 ==========
#define DBG_LOG(msg) std::cout << "[PROXY_DBG] " << msg << std::endl

// ========== 线程参数包 ==========
// _beginthreadex 只能传一个 void*，所以把"服务器对象"和"浏览器连接"打包
struct ThreadParams {
    ProxyServer* server;
    int client;
};

// ========== 线程工作函数（全局静态，类外定义）==========
// 因为 handleClient 已经改成 public 了，这里可以正常调用
static unsigned int __stdcall clientWorker(void* param)
{
    ThreadParams* p = (ThreadParams*)param;
    p->server->handleClient(p->client);  // 调用 ProxyServer 的处理逻辑
    delete p;                          // 释放 new 出来的内存，防止泄漏
    return 0;
}

// ========== 解析 CONNECT 请求里的目标地址 ==========
// 例如 "CONNECT www.baidu.com:443 HTTP/1.1"
// 提取出 host="www.baidu.com", port=443
static bool parseConnectTarget(const std::string& firstLine, std::string& host, int& port)
{
    size_t s1 = firstLine.find(' ');
    if (s1 == std::string::npos) return false;
    size_t s2 = firstLine.find(' ', s1 + 1);
    if (s2 == std::string::npos) return false;

    std::string target = firstLine.substr(s1 + 1, s2 - s1 - 1);

    size_t colon = target.find(':');
    if (colon != std::string::npos) {
        host = target.substr(0, colon);
        port = atoi(target.substr(colon + 1).c_str());
    } else {
        host = target;
        port = 443;
    }
    return !host.empty();
}

// ========== 【关键修复】循环发送，确保数据全部发完 ==========
// 原来的 bug：send() 可能只发了一部分（比如 4096 字节只发了 1500），
// 导致浏览器或服务器永远等不到完整数据，一直卡住。
static bool sendAll(SOCKET sock, const char* buf, int len)
{
    int totalSent = 0;
    while (totalSent < len) {
        int sent = send(sock, buf + totalSent, len - totalSent, 0);
        if (sent == SOCKET_ERROR || sent <= 0) return false;
        totalSent += sent;
    }
    return true;
}

// ========== 连接真实的目标服务器 ==========
static SOCKET connectToHost(const std::string& host, int port)
{
    DBG_LOG("开始连接真实服务器: " << host << ":" << port);

    // 先判断是不是纯 IP 地址（如 127.0.0.1）
    unsigned long ipAddr = inet_addr(host.c_str());
    if (ipAddr == INADDR_NONE) {
        // 是域名，用 DNS 解析
        struct hostent* he = gethostbyname(host.c_str());
        if (!he) {
            DBG_LOG("DNS 解析失败: " << host);
            return INVALID_SOCKET;
        }
        ipAddr = ((struct in_addr*)he->h_addr)->s_addr;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ipAddr;

    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        DBG_LOG("connect 失败，错误码=" << WSAGetLastError());
        closesocket(s);
        return INVALID_SOCKET;
    }

    DBG_LOG("连接真实服务器成功");
    return s;
}

// ========== 双向透明转发（传话筒）==========
static void bidirectionalRelay(SOCKET client, SOCKET server)
{
    fd_set fds;
    char buffer[4096];
    timeval tv;
    int idleLoops = 0;

    while (true) {
        FD_ZERO(&fds);
        FD_SET(client, &fds);
        FD_SET(server, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(0, &fds, NULL, NULL, &tv);
        if (ret < 0) break;
        if (ret == 0) {
            idleLoops++;
            if (idleLoops > 30) break; // 30秒没动静就断开
            continue;
        }
        idleLoops = 0;

        if (FD_ISSET(client, &fds)) {
            int n = recv(client, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            if (!sendAll(server, buffer, n)) break;
        }

        if (FD_ISSET(server, &fds)) {
            int n = recv(server, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            if (!sendAll(client, buffer, n)) break;
        }
    }
}

// =========================================================================
// 类成员函数
// =========================================================================

ProxyServer::ProxyServer(int port, FilterEngine& engine, BlockLogFile& logger)
	: port_(port), engine_(engine), logger_(logger),
	  listenSocket_(INVALID_SOCKET), running_(false), threadHandle_(NULL)
{
}

ProxyServer::~ProxyServer()
{
	stop();
}

bool ProxyServer::start()
{
	listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket_ == INVALID_SOCKET) return false;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port_);

	if (bind(listenSocket_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(listenSocket_);
		listenSocket_ = INVALID_SOCKET;
		return false;
	}

	if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(listenSocket_);
		listenSocket_ = INVALID_SOCKET;
		return false;
	}

	running_ = true;
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0, threadFunc, this, 0, NULL);
	if (threadHandle_ == NULL)
	{
		running_ = false;
		closesocket(listenSocket_);
		listenSocket_ = INVALID_SOCKET;
		return false;
	}

	return true;
}

void ProxyServer::stop()
{
	running_ = false;
	if (listenSocket_ != INVALID_SOCKET)
	{
		closesocket(listenSocket_);
		listenSocket_ = INVALID_SOCKET;
	}
	if (threadHandle_ != NULL)
	{
		WaitForSingleObject(threadHandle_, 2000);
		CloseHandle(threadHandle_);
		threadHandle_ = NULL;
	}
}

bool ProxyServer::isRunning() const
{
	return running_;
}

unsigned int __stdcall ProxyServer::threadFunc(void* param)
{
	ProxyServer* server = (ProxyServer*)param;
	server->runLoop();
	return 0;
}

void ProxyServer::runLoop()
{
	while (running_)
	{
		sockaddr_in clientAddr;
		int len = sizeof(clientAddr);
		int client = accept(listenSocket_, (sockaddr*)&clientAddr, &len);
		
		if (client == INVALID_SOCKET) break;
		
		// 【兼容旧编译器的写法】
		ThreadParams* p = new ThreadParams;
		p->server = this;
		p->client = client;
		
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, clientWorker, p, 0, NULL);
		if (h == NULL) {
			closesocket(client);
			delete p;
		} else {
			CloseHandle(h);
		}
	}
}

// =========================================================================
// 【核心处理函数】每个浏览器连接都会有一个独立线程执行这个函数
// =========================================================================
void ProxyServer::handleClient(int clientSocket)
{
    DBG_LOG("========== 新连接进入 ==========");

    // 第 1 步：读浏览器发来的请求头
    char buf[4096];
    int n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (n <= 0)
    {
        DBG_LOG("recv 失败，浏览器没发数据就关了，n=" << n);
        closesocket(clientSocket);
        return;
    }
    buf[n] = '\0';

    std::string request(buf);
    DBG_LOG("收到请求前 120 字: " << request.substr(0, 120));

    // 提取第一行（请求行）
    size_t firstLineEnd = request.find("\r\n");
    std::string firstLine = request.substr(0, firstLineEnd);
    DBG_LOG("请求行: " << firstLine);

    // =====================================================================
    // 分支 A：HTTPS CONNECT 隧道（访问 https://... 时走这里）
    // =====================================================================
    if (firstLine.find("CONNECT") == 0)
    {
        std::string targetHost;
        int targetPort = 443;
        if (!parseConnectTarget(firstLine, targetHost, targetPort))
        {
            DBG_LOG("CONNECT 解析失败");
            closesocket(clientSocket);
            return;
        }
        DBG_LOG("CONNECT 目标: " << targetHost << ":" << targetPort);

        // 用域名去匹配过滤规则（HTTPS 只能看到域名，看不到 URL 路径）
        std::string url = "https://" + targetHost;
        std::string hitRule;
        bool blocked = engine_.shouldBlock(url, targetHost, hitRule);
        DBG_LOG("过滤结果: blocked=" << (blocked ? "是" : "否") << " 规则=" << hitRule);

        if (blocked)
        {
            // 命中广告规则，直接返回 403
            logger_.log(BLOCKED, hitRule, url);
            std::string resp = "HTTP/1.1 403 Forbidden\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 22\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "<h1>Ad Blocked</h1>";
            sendAll(clientSocket, resp.c_str(), resp.length());
            closesocket(clientSocket);
            DBG_LOG("已拦截 CONNECT");
            return;
        }

        // 放行：连接真实服务器
        SOCKET serverSocket = connectToHost(targetHost, targetPort);
        if (serverSocket == INVALID_SOCKET)
        {
            closesocket(clientSocket);
            return;
        }

        logger_.log(ALLOWED, hitRule, url);

        // 告诉浏览器"隧道已建立"（HTTP 代理协议规定）
        const char* established = "HTTP/1.1 200 Connection Established\r\n\r\n";
        if (!sendAll(clientSocket, established, strlen(established))) {
            closesocket(serverSocket);
            closesocket(clientSocket);
            return;
        }
        DBG_LOG("发送 200 Established，开始双向转发...");

        // 之后浏览器和服务器直接加密通信，我只负责传数据
        bidirectionalRelay(clientSocket, serverSocket);

        closesocket(serverSocket);
        closesocket(clientSocket);
        DBG_LOG("CONNECT 处理结束");
        return;
    }

    // =====================================================================
    // 分支 B：普通 HTTP 请求（访问 http://... 时走这里）
    // =====================================================================

    // 从 Host 头提取域名
    std::string host = "unknown";
    size_t hostPos = request.find("Host: ");
    if (hostPos != std::string::npos)
    {
        hostPos += 6; // 跳过 "Host: "
        size_t hostEnd = request.find("\r\n", hostPos);
        if (hostEnd != std::string::npos)
            host = request.substr(hostPos, hostEnd - hostPos);
    }
    DBG_LOG("提取到 Host: " << host);

    // 构造完整 URL（http://域名 + 路径）
    std::string url = "http://" + host;
    size_t pathStart = firstLine.find(' ');
    if (pathStart != std::string::npos)
    {
        size_t pathEnd = firstLine.find(' ', pathStart + 1);
        if (pathEnd != std::string::npos)
            url += firstLine.substr(pathStart + 1, pathEnd - pathStart - 1);
    }
    DBG_LOG("完整 URL: " << url);

    // 调用过滤引擎判断
    std::string hitRule;
    bool blocked = engine_.shouldBlock(url, host, hitRule);
    DBG_LOG("过滤结果: blocked=" << (blocked ? "是" : "否") << " 规则=" << hitRule);

    if (blocked)
    {
        logger_.log(BLOCKED, hitRule, url);
        std::string response = "HTTP/1.1 403 Forbidden\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: 22\r\n"
                              "Connection: close\r\n"
                              "\r\n"
                              "<h1>Ad Blocked</h1>";
        sendAll(clientSocket, response.c_str(), response.length());
        closesocket(clientSocket);
        DBG_LOG("已拦截 HTTP");
        return;
    }

    // 放行：解析域名和端口，连接真实服务器
    int targetPort = 80;
    std::string targetHost = host;
    size_t colon = host.find(':');
    if (colon != std::string::npos)
    {
        targetHost = host.substr(0, colon);
        targetPort = atoi(host.substr(colon + 1).c_str());
    }

    DBG_LOG("准备连接真实服务器: " << targetHost << ":" << targetPort);
    SOCKET serverSocket = connectToHost(targetHost, targetPort);
    if (serverSocket == INVALID_SOCKET)
    {
        closesocket(clientSocket);
        return;
    }

    logger_.log(ALLOWED, hitRule, url);

    // 把浏览器原始请求原样发给服务器
    DBG_LOG("转发浏览器请求到服务器...");
    if (!sendAll(serverSocket, request.c_str(), request.length())) {
        closesocket(serverSocket);
        closesocket(clientSocket);
        return;
    }

    // 进入双向转发：把服务器的响应传回浏览器
    bidirectionalRelay(clientSocket, serverSocket);

    closesocket(serverSocket);
    closesocket(clientSocket);
    DBG_LOG("HTTP 处理结束");
}