#include "../include/TaskQueue.h"

void TaskQueue::push(int clientSocket)
{
    std::unique_lock<std::mutex> lock(mtx);

    tasks.push(clientSocket);

    cv.notify_one();
}

int TaskQueue::pop()
{
    std::unique_lock<std::mutex> lock(mtx);

    while(tasks.empty())
    {
        cv.wait(lock);
    }

    int socket = tasks.front();

    tasks.pop();

    return socket;
}