#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

class TaskQueue
{
private:
    std::queue<int> tasks;

    std::mutex mtx;

    std::condition_variable cv;

public:
    void push(int clientSocket);

    int pop();
};

#endif