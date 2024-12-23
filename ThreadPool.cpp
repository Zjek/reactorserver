#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum, const std::string& threadtype):stop_(false), threadtype_(threadtype)
{
    for(size_t i = 0; i < threadnum; ++i)
    {
        threads_.emplace_back([this]{
            printf("create %s thraed(%ld)\n", threadtype_.c_str(),syscall(SYS_gettid));

            while(stop_ == false)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    this->condition_.wait(lock, [this]
                    {
                        return ((this->stop_ == true) || (this->taskqueue_.empty() == false));
                    });

                    if((this->stop_ == true) && (this->taskqueue_.empty() == true)) return;

                    task = std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }

                printf("%s(%ld) execute task.\n", threadtype_.c_str(),syscall(SYS_gettid));
                task();
            }
        });
    }
}
void ThreadPool::addtask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }

    condition_.notify_one();
}
size_t ThreadPool::size()
{
    return threads_.size();
}
ThreadPool::~ThreadPool()
{
   stop();
}
void ThreadPool::stop()
{
    if(stop_) return;

    stop_ = true;

    condition_.notify_all();
    for(std::thread &th:threads_)
        th.join();
}

