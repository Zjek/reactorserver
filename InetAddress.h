#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress{
private:
    sockaddr_in addr_;
public:
    InetAddress() = default;
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const sockaddr_in addr);
    ~InetAddress();

    const char *ip() const;         
    uint16_t    port() const;
    const sockaddr *addr() const;   //返回addr_成员地址，转换成了sockaddr
    void setaddr(sockaddr_in clientaddr);
};

#endif