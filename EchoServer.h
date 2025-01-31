#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include <functional>
#include "ThreadPool.h"

class EchoServer{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;
public:
    EchoServer(const std::string &ip, const uint16_t port, int subthreadnum = 3, int workthreadnum = 5);
    ~EchoServer();

    void Start();
    void Stop();
    
    void HandleNewConnection(spConnection clientsock);
    void HandleClose(spConnection conn);
    void HandleError(spConnection conn);
    void HandleMessage(spConnection conn, std::string& message);
    void HandleSendComplete(spConnection conn);
    void HandleTimeOut(EventLoop *loop);

    void OnMessage(spConnection conn, std::string& message);
};


#endif