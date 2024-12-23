#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <sys/syscall.h>

class ThreadPool
{
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> taskqueue_;
    std::mutex  mutex_;
    std::condition_variable condition_;
    std::atomic_bool stop_;
    const std::string threadtype_;

public:
    ThreadPool(size_t threadnum, const std::string& threadtype);
    void addtask(std::function<void()> task);
    size_t size();
    ~ThreadPool();

    void stop();
};





#endif