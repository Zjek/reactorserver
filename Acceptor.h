#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"

class EventLoop;
class Channel;
class Socket;

class Acceptor{
private:
    EventLoop* loop_;
    Socket  servsock_;
    Channel acceptchannel_;
    std::function<void (std::unique_ptr<Socket>)> newconnectioncb_;
public:
    Acceptor(EventLoop* loop, const std::string &ip, const uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconnectioncb(std::function<void (std::unique_ptr<Socket>)> fn);
};

#endif