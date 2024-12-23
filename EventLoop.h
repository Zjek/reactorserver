#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include "Epoll.h"
#include <functional>
#include "Channel.h"
#include <memory>
#include <atomic>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <queue>
#include <mutex>
#include "Timestamp.h"
#include <map>
#include "Connection.h"


class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;

class EventLoop{
private:
    int timetvl_;                               //闹钟时间间隔
    int timeout_;                               //Connection对象超时时间
    std::unique_ptr<Epoll> ep_;
    std::function<void (EventLoop *)> epolltimeoutcallback_;
    pid_t threadid_;
    std::queue<std::function<void()>> taskqueue_;   //事件循环线程被eventfd唤醒后执行的任务
    std::mutex  mutex_;
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;
    int timerfd_;
    std::unique_ptr<Channel> timerchannel_;
    bool mainloop_;                             //true是主事件循环，false是从事件循环
    std::mutex mmutex_;
    std::map<int, spConnection> conns_;
    std::function<void(int)> timercallback_;
    std::atomic_bool stop_;

public:
    EventLoop(bool mainloop, int timetvl = 30, int timeout = 80);
    ~EventLoop();
    void run();
    void stop();


    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void (EventLoop *)> func);
    void settimercallback(std::function<void(int)> func);

    bool isinloopthread();

    void queueinloop(std::function<void()> func);
    void wakeup();
    void handlewakeup();

    void handletimer();
    void newconnection(spConnection conn);
};

#endif