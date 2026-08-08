#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>

#include "TaskQueue.h"

class ThreadPool
{
private:

    TaskQueue queue;

    std::vector<std::thread> workers;

    void worker();

public:

    ThreadPool(int numThreads);

    void enqueue(int clientSocket);
};

#endif