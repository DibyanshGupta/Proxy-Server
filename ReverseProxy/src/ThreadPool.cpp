#include "../include/ThreadPool.h"
#include "../include/ClientHandler.h"

ThreadPool::ThreadPool(int num,
                       TaskQueue &q,
                       Router &r)
    : queue(q),
      router(r)
{
    for (int i = 0; i < num; i++)
    {
        workers.push_back(
            std::thread(&ThreadPool::work,
                        this));
    }
}

void ThreadPool::work()
{
    while (true)
    {
        int client = queue.pop();

        if (client == -1)
        {
            break;
        }

        handleClient(client,
                     router);
    }
}

ThreadPool::~ThreadPool()
{
    queue.shutdown();

    for (auto &t : workers)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}