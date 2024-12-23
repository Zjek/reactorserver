#include "EventLoop.h"

int createtimerfd(int sec = 5)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

EventLoop::EventLoop(bool mainloop, int timetvl, int timeout)
         :timetvl_(timetvl), timeout_(timeout), ep_(new Epoll), wakeupfd_(eventfd(0, EFD_NONBLOCK)), 
         wakechannel_(new Channel(this, wakeupfd_)),timerfd_(createtimerfd(timeout_)), 
         timerchannel_(new Channel(this, timerfd_)), mainloop_(mainloop), stop_(false)
        
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    wakechannel_->enablereading();
    
    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer, this));
    timerchannel_->enablereading();
    
}
EventLoop::~EventLoop()
{
 
}
void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid);
    while(stop_ == false)
    {
        std::vector<Channel*> channels = ep_->loop();

        if(channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for(auto &ch:channels)
            {
                ch->handleevent();
            }
        }
    }
}

void EventLoop::stop()
{
    stop_ = true;
    wakeup();
}


void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void (EventLoop *)> func)
{
    epolltimeoutcallback_ = func;
}

bool EventLoop::isinloopthread()
{
    return (threadid_ == syscall(SYS_gettid));
}

void EventLoop::queueinloop(std::function<void()> func)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(func);
    }
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));
}

void EventLoop::handlewakeup()
{
    printf("handlewakeup() thread id:%ld\n", syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val));

    std::function<void()> func;
    std::lock_guard<std::mutex> gd(mutex_);

    while(taskqueue_.size() > 0)
    {
        func = std::move(taskqueue_.front());
        taskqueue_.pop();
        func();
    }
}

void EventLoop::handletimer()
{
    struct  itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timetvl_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_, 0, &timeout, 0);
    if(mainloop_)
    {
        //主事件循环
    }
    else
    {
        //从事件循环
        time_t now = time(0);
        for(auto aa:conns_)
        {
            if(aa.second->timeout(now, timeout_))
            {
                {
                    std::lock_guard<std::mutex> gd(mmutex_);
                    conns_.erase(aa.first);
                }
                timercallback_(aa.first);
            }
        }
    }
}

void EventLoop::newconnection(spConnection conn)
{
    std::lock_guard<std::mutex> gd(mmutex_);
    conns_[conn->fd()] = conn;
}

void EventLoop::settimercallback(std::function<void(int)> func)
{
    timercallback_ = func;
}
