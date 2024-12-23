#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <string>
#include <iostream>
#include <cstring>

class Buffer{
private:
    std::string buf_;
    const uint16_t sep_;    //报文分隔符，0.无分隔符（固定长度，视频会议）； 1.四字节报头 2.\r\n\r\n分隔符（http协议）
public:
    Buffer(uint16_t sep = 1);
    ~Buffer();

    void append(const char *data, size_t size);
    void appendwithsep(const char*data, size_t size);
    size_t size();
    const char* data();
    void clear();
    void erase(size_t pos, size_t nn);
    bool pickmessage(std::string &ss);
};

#endif