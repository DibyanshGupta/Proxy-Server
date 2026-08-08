#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>

#include "TaskQueue.h"
#include "ClientHandler.h"

class ThreadPool
{
private:
    TaskQueue queue;

    std::vector<std::thread> workers;

    ClientHandler clientHandler;

    void worker();

public:
    ThreadPool(
        int numThreads,
        Database& database
    );

    void enqueue(int clientSocket);
};

#endif