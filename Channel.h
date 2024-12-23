#ifndef __CHANNEL_H__
#define __CHANNEL_H__
#include "EventLoop.h"
#include <sys/epoll.h>
#include "InetAddress.h"
#include "Socket.h"
#include <functional>
#include "Connection.h"
#include <memory>

class Channel
{
private:
    int fd_ = -1;
    EventLoop* loop_;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;

    std::function<void()> readcallback_;    
    std::function<void()> writecallback_;
    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;


public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int fd();
    void useet();
    void enablereading();
    void disablereading();
    void enablewriting();
    void disablewriting();
    void disableall();
    void remove();
    void setinepoll(bool inepoll);
    void setrevents(uint32_t ev);
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void handleevent();
    void setreadcallback(std::function<void()> func);
    void setwritecallback(std::function<void()> func);
    void setclosecallback(std::function<void()> func);
    void seterrorcallback(std::function<void()> func);
    
};


#endif