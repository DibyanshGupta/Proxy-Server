#include "../include/TaskQueue.h"

void TaskQueue::push(int clientSocket)
{
    {
        std::lock_guard<std::mutex> lock(mtx);

        tasks.push(clientSocket);
    }

    cv.notify_one();
}

int TaskQueue::pop()
{
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]()
    {
        return !tasks.empty();
    });

    int clientSocket = tasks.front();

    tasks.pop();

    return clientSocket;
}