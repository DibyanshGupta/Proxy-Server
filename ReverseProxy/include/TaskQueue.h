#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

class TaskQueue
{
private:
    std::queue<int> q;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;

public:
    TaskQueue();

    void push(int client);

    int pop();

    void shutdown();
};

#endif