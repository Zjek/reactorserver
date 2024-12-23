#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>
#include <unistd.h>
#include <sys/syscall.h>
#include "Timestamp.h"

class Timestamp;
class Connection;
using spConnection = std::shared_ptr<Connection>;
class EventLoop;
class Socket;
class Channel;
class Connection: public std::enable_shared_from_this<Connection>
{
private:

    EventLoop* loop_;
    std::unique_ptr<Socket>  clientsock_;
    std::unique_ptr<Channel> clientchannel_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool disconnect_;

    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection)> errorcallback_;
    std::function<void(spConnection, std::string&)> onmessagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;
    Timestamp lasttatime_;

public:
    Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock);
    ~Connection();
    int fd() const;
    std::string ip()const;
    uint16_t    port() const;


    void onmessage();
    void closecallback();
    void errorcallback();
    void writecallback();


    void setclosecallback(std::function<void(spConnection)> func);
    void seterrorcallback(std::function<void(spConnection)> func);
    void setonmessagecallback(std::function<void(spConnection, std::string&)> func);
    void setsendcompletecallback(std::function<void(spConnection)> func);
    //不管任何线程都会调用此函数发送数据
    void send(const char* data, size_t size);
    //如果当前是IO线程，直接调用此函数，如果是工作线程，将此函数传给IO线程
    void sendinloop(const char* data, size_t size);
    bool timeout(time_t now, int val);
};

#endif