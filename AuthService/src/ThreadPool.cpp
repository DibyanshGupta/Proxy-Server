#include "../include/ThreadPool.h"

ThreadPool::ThreadPool(
    int numThreads,
    Database& database
)
    : clientHandler(database)
{
    for (int i = 0; i < numThreads; i++)
    {
        workers.emplace_back(
            &ThreadPool::worker,
            this
        );
    }
}

void ThreadPool::enqueue(int clientSocket)
{
    queue.push(clientSocket);
}

void ThreadPool::worker()
{
    while (true)
    {
        int clientSocket = queue.pop();

        clientHandler.handleClient(clientSocket);
    }
}