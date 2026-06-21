#include "TestServer.h"
#include <iostream>
#include <string>
#include <process.h>

TestServer::TestServer(int port)
    : port_(port), listenSocket_(INVALID_SOCKET), running_(false), threadHandle_(NULL)
{
}

TestServer::~TestServer()
{
    stop();
}

bool TestServer::start()
{
    listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket_ == INVALID_SOCKET)
    {
        return false;
    }

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

void TestServer::stop()
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

bool TestServer::isRunning() const
{
    return running_;
}

unsigned int __stdcall TestServer::threadFunc(void* param)
{
    TestServer* server = (TestServer*)param;
    server->runLoop();
    return 0;
}

void TestServer::runLoop()
{
    while (running_)
    {
        sockaddr_in clientAddr;
        int len = sizeof(clientAddr);

        int client = accept(listenSocket_, (sockaddr*)&clientAddr, &len);
        if (client == INVALID_SOCKET)
        {
            break;
        }

        handleClient(client);
        closesocket(client);
    }
}

void TestServer::sendResponse(int clientSocket, int code, const std::string& contentType, const std::string& body)
{
    std::string status = (code == 200) ? "200 OK" : "404 Not Found";

    char lenStr[32];
    sprintf(lenStr, "%d", (int)body.length());

    std::string header = "HTTP/1.1 " + status + "\r\n"
                         "Content-Type: " + contentType + "\r\n"
                         "Content-Length: " + std::string(lenStr) + "\r\n"
                         "Connection: close\r\n"
                         "\r\n";

    send(clientSocket, header.c_str(), header.length(), 0);
    send(clientSocket, body.c_str(), body.length(), 0);
}

void TestServer::handleClient(int clientSocket)
{
    char buf[4096];
    int n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return;
    buf[n] = '\0';

    std::string request(buf);

    // 追踪脚本（保持这个，用于关键词拦截测试）
    if (request.find("GET /track.js") != std::string::npos)
    {
        std::string js = "console.log('Tracking script loaded - this should be blocked');";
        sendResponse(clientSocket, 200, "application/javascript", js);
        return;
    }

    // 其他所有请求都返回 normal.html（包括根路径、任意路径）
    std::string html =
        "<!DOCTYPE html>"
        "<html><head><title>网页广告拦截测试</title>"
        "<style>"
        "body{font-family:sans-serif; padding:20px; max-width:900px; margin:0 auto;}"
        "h1{color:#333; border-bottom:2px solid #4CAF50; padding-bottom:10px;}"
        ".section{border:2px solid #ddd; margin:15px 0; padding:15px; border-radius:5px;}"
        ".ad-section{border-color:#ff4444; background:#fff0f0;}"
        ".normal-section{border-color:#44aa44; background:#f0fff0;}"
        ".blocked{color:#ff4444; font-weight:bold; text-align:center; padding:10px;}"
        "img{max-width:200px; height:auto; display:block; margin:10px auto;}"
        ".ad-card{border:1px dashed #ff4444; padding:10px; text-align:center; color:#ff4444; display:inline-block; margin:5px; min-width:150px; min-height:100px;}"
        ".normal-card{border:1px solid #4CAF50; padding:10px; text-align:center; color:#2e7d32; display:inline-block; margin:5px; min-width:150px; min-height:100px; background:#e8f5e9;}"
        "</style></head>"
        "<body>"
        "<h1>网页广告拦截测试</h1>"
        
        // 正常内容区域
        "<div class='section normal-section'>"
        "<h2>[正常内容区域]</h2>"
        "<p>这是一篇正常的文章，图片应该正常显示：</p>"
        "<div class='normal-card'>"
        "  <img src='data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMzAwIiBoZWlnaHQ9IjIwMCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48cmVjdCB3aWR0aD0iMzAwIiBoZWlnaHQ9IjIwMCIgZmlsbD0iIzRDQUY1MCIvPjx0ZXh0IHg9IjE1MCIgeT0iMTAwIiBmb250LXNpemU9IjIwIiBmaWxsPSJ3aGl0ZSIgdGV4dC1hbmNob3I9Im1pZGRsZSIgZHk9Ii4zZW0iPk5vcm1hbCBJbWFnZTwvdGV4dD48L3N2Zz4='>"
        "  <p>正常图片 - 代理放行</p>"
        "</div>"
        "<p>上面这张图片来自正常服务器，代理不会拦截，正常显示。</p>"
        "</div>"
        
        // 广告区域 - 域名拦截
        "<div class='section ad-section'>"
        "<h2>[域名广告区域]（按 doubleclick.net 拦截）</h2>"
        "<p>下面的图片来自广告服务器，应该被拦截显示占位：</p>"
        "<div style='display:flex; gap:10px;'>"
        "  <div class='ad-card'>"
        "    <img src='http://doubleclick.net/ad1.jpg' onerror=\"this.style.display='none'; this.parentElement.innerHTML='<p class=blocked>[X] 域名广告被拦截</p>'\">"
        "  </div>"
        "  <div class='ad-card'>"
        "    <img src='http://doubleclick.net/ad2.jpg' onerror=\"this.style.display='none'; this.parentElement.innerHTML='<p class=blocked>[X] 域名广告被拦截</p>'\">"
        "  </div>"
        "</div>"
        "</div>"
        
        // 广告区域 - 关键词拦截
        "<div class='section ad-section'>"
        "<h2>[脚本广告区域]（按关键词 /tracker /popup 拦截）</h2>"
        "<p>下面的脚本来自追踪服务器，应该被拦截：</p>"
        "<div style='display:flex; gap:10px;'>"
        "  <div class='ad-card'>"
        "    <script src='http://example.com/tracker.js' onerror=\"this.parentElement.innerHTML='<p class=blocked>[X] 跟踪脚本被拦截</p>'\"></script>"
        "  </div>"
        "  <div class='ad-card'>"
        "    <script src='http://example.com/popup.js' onerror=\"this.parentElement.innerHTML='<p class=blocked>[X] 弹窗脚本被拦截</p>'\"></script>"
        "  </div>"
        "</div>"
        "</div>"
        
        "</body></html>";

    sendResponse(clientSocket, 200, "text/html", html);
}