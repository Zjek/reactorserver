#include "TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port, int threadnum)
        :threadnum_(threadnum), mainloop_(new EventLoop(true)), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "IO")
{
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimout, this, std::placeholders::_1));
    
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection, this,std::placeholders::_1));

    for(int i = 0; i < threadnum_; ++i)
    {
        subloops_.emplace_back(new EventLoop(false, 5, 10));
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimout, this, std::placeholders::_1));
        subloops_[i]->settimercallback(std::bind(&TcpServer::removeconn, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}
TcpServer::~TcpServer()
{


}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::stop()
{
    mainloop_->stop();
    
    for(int i = 0; i < threadnum_; ++i) subloops_[i]->stop();

    threadpool_.stop();
}

void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
    spConnection conn(new Connection(subloops_[clientsock->fd() % threadnum_].get(), std::move(clientsock)));
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    //printf("accept clientfd(fd=%d,ip=%s,port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[conn->fd()] = conn;
    }
    subloops_[conn->fd() % threadnum_]->newconnection(conn);

    if(newconnectioncb_) newconnectioncb_(conn);
}

void TcpServer::closeconnection(spConnection conn)
{
    if(closeconnectioncb_) closeconnectioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mmutex_);       
        conns_.erase(conn->fd());
    }
}

void TcpServer::errorconnection(spConnection conn)
{
    if(errorconnectioncb_) errorconnectioncb_(conn);
    {
        std::lock_guard<std::mutex> gd(mmutex_);       
        conns_.erase(conn->fd());
    }
}

void TcpServer::onmessage(spConnection conn, std::string &message)
{
    if(onmessagecb_) onmessagecb_(conn, message);
}
void TcpServer::sendcomplete(spConnection conn)
{
    if(sendcompletecb_) sendcompletecb_(conn);
}

void TcpServer::epolltimout(EventLoop *loop)
{
    if(timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> func)
{
    newconnectioncb_ = func;
}
void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> func)
{
    closeconnectioncb_ = func;
}
void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> func)
{
    errorconnectioncb_ = func;
}
void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> func)
{
    onmessagecb_ = func;
}
void TcpServer::setsendcompletecb(std::function<void(spConnection)> func)
{
    sendcompletecb_ = func;
}
void TcpServer::settimeoutcb(std::function<void(EventLoop *)> func)
{
    timeoutcb_ = func;
}

void TcpServer::removeconn(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(fd);
    }
}

