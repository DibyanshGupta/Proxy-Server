#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>

#include "TaskQueue.h"
#include "Router.h"

class ThreadPool
{
private:

    std::vector<std::thread> workers;

    TaskQueue &queue;

    Router &router;

    void work();

public:

    ThreadPool(int num,
               TaskQueue &q,
               Router &r);

    ~ThreadPool();
};

#endif