#include "Socket.h"

int createnonblocking()
{
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd < 0)
    {
        perror("socket() failed"); 
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);exit(-1);
    }

    return listenfd;
}


Socket::Socket(int fd):fd_(fd) 
{

}
Socket::~Socket()
{
    ::close(fd_);
}

int Socket::fd() const
{
    return fd_;
}
void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}
void Socket::setreuseport(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

}
void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}
void Socket::setkeepalive(bool on)
{
    int optval = on ? 1: 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
void Socket::bind(const InetAddress &servaddr)
{
    if(::bind(fd_, servaddr.addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed");close(fd_); exit(-1);
    }

    setipport(servaddr.ip(), servaddr.port());

}
void Socket::listen(int nn)
{
    if(::listen(fd_, nn) != 0)
    {
        perror("listen() failed"); close(fd_); exit(-1);
    }
}
int Socket::accept(InetAddress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(fd_, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);
   

    return clientfd;
}
std::string Socket::ip()const
{
    return ip_;
}
uint16_t    Socket::port() const
{
    return port_;
}

void Socket::setipport(const std::string& ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
}