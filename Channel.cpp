#include "Channel.h"

Channel::Channel(EventLoop* loop, int fd):loop_(loop), fd_(fd)
{

}
Channel::~Channel()
{

}
int Channel::fd()
{
    return fd_;
}
void Channel::useet()
{
    events_ |= EPOLLET;
}
void Channel::enablereading()
{
    events_ |= EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
    loop_->updatechannel(this);
}
void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
    loop_->updatechannel(this);
}
void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disableall()
{
    events_ = 0;
    loop_->updatechannel(this);
}
void Channel::remove()
{
    disableall();
    loop_->removechannel(this);
}

void Channel::setinepoll(bool inepoll)
{
    inepoll_ = inepoll;
}
void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}
bool Channel::inpoll()
{
    return inepoll_;
}
uint32_t Channel::events()
{
    return events_;
}
uint32_t Channel::revents()
{
    return revents_;
}

void Channel::handleevent()
{
    if(revents_ & EPOLLRDHUP)
    {
        closecallback_();
    }
    else if(revents_ & (EPOLLIN|EPOLLPRI))
    {
        readcallback_();
    }
    else if(revents_ & EPOLLOUT)
    {
        writecallback_();
    }
    else
    {
        errorcallback_();
    }
}





void Channel::setreadcallback(std::function<void()> func)
{
    readcallback_ = func;
}
void Channel::setwritecallback(std::function<void()> func)
{
    writecallback_ = func;
}
void Channel::setclosecallback(std::function<void()> func)
{
    closecallback_ = func;
}
void Channel::seterrorcallback(std::function<void()> func)
{
    errorcallback_ = func;
}


