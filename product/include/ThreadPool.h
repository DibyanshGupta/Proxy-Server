#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>

#include "TaskQueue.h"

class ThreadPool
{
private:

    std::vector<std::thread> workers;
    TaskQueue &queue;
    bool run;

    void work();

public:

    ThreadPool(int num, TaskQueue &q);

    ~ThreadPool();
};

#endif