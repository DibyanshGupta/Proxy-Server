#include "../include/TaskQueue.h"
#include "../include/ThreadPool.h"
#include "../include/Server.h"

#include <thread>

int main()
{
    TaskQueue queue;

    int numThreads = std::thread::hardware_concurrency();

    if (numThreads == 0)
    {
        numThreads = 4;
    }

    ThreadPool pool(numThreads, queue);

    Server server(9003, queue);

    if (!server.start())
    {
        return 1;
    }

    server.run();

    return 0;
}