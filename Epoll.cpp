#include "Epoll.h"

Epoll::Epoll()
{
    if((epollfd_ = ::epoll_create(1)) == -1)
    {
        printf("epoll_create() failed(%d)\n", errno);exit(-1);
    }

}
Epoll::~Epoll()
{
    close(epollfd_);
}

/*void Epoll::addfd(int fd, uint32_t op)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;
    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll_ctl() failed(%d)", errno); exit(-1);
    }
}
std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents,timeout);
    if(infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }
    if(infds == 0)
    {
        printf("epoll_wait() timeout\n"); return evs;
    }

    for(int i = 0; i < infds; i++)
    {
        evs.push_back(events_[i]);
    }
    return evs;
}*/

std::vector<Channel*> Epoll::loop(int timeout)
{
    std::vector<Channel*> channels;
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents,timeout);
    if(infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }
    if(infds == 0)
    {
        return channels;
    }

    for(int i = 0; i < infds; i++)
    {
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        channels.push_back(ch);
    }
    return channels;
}
void Epoll::removechannel(Channel *ch)
{
    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_DEL, ch->fd(), 0) == -1)
        {
            perror("epoll_ctl() failed\n"); exit(-1);
        }
    }
}

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();

    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() failed\n"); exit(-1);
        }
    }
    else
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl() failed\n"); exit(-1);
        }
        ch->setinepoll(true);
    }
}