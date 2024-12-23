#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <string>
#include <time.h>
#include <sys/timerfd.h>

class Timestamp{
private:
    time_t secsinceepoch_;
public:
    Timestamp();
    Timestamp(int64_t secsinceepoch);
    ~Timestamp();


    static Timestamp now();

    time_t toint() const;
    std::string tostring() const;
};

#endif