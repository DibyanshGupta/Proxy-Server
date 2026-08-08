#include "../include/TaskQueue.h"

TaskQueue::TaskQueue()
{
    stop = false;
}

void TaskQueue::push(int client)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(client);
    }

    cv.notify_one();
}

int TaskQueue::pop()
{
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]()
    {
        return stop || !q.empty();
    });

    if (stop && q.empty())
    {
        return -1;
    }

    int client = q.front();

    q.pop();

    return client;
}

void TaskQueue::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }

    cv.notify_all();
}