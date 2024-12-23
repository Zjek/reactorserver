#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <functional>
#include"ThreadPool.h"
#include <memory>
#include <mutex>


class TcpServer{
private:
    int threadnum_;
    std::unique_ptr<EventLoop> mainloop_;
    std::vector<std::unique_ptr<EventLoop>> subloops_;
    Acceptor acceptor_;
    ThreadPool threadpool_;
    std::mutex mmutex_;
    std::map<int, spConnection> conns_;
    std::function<void(spConnection)> newconnectioncb_;
    std::function<void(spConnection)> closeconnectioncb_;
    std::function<void(spConnection)> errorconnectioncb_;
    std::function<void(spConnection, std::string &message)> onmessagecb_;
    std::function<void(spConnection)> sendcompletecb_;
    std::function<void(EventLoop *)> timeoutcb_;

public:
    TcpServer(const std::string& ip, const uint16_t port, int threadnum = 3);
    ~TcpServer();
    void start();
    void stop();
    void newconnection(std::unique_ptr<Socket> clientsock);
    void closeconnection(spConnection conn);
    void errorconnection(spConnection conn);
    void onmessage(spConnection conn, std::string &message);
    void sendcomplete(spConnection conn);
    void epolltimout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(spConnection)> func);
    void setcloseconnectioncb(std::function<void(spConnection)> func);
    void seterrorconnectioncb(std::function<void(spConnection)> func);
    void setonmessagecb(std::function<void(spConnection, std::string &message)> func);
    void setsendcompletecb(std::function<void(spConnection)> func);
    void settimeoutcb(std::function<void(EventLoop *)> func);

    void removeconn(int fd);

};

#endif