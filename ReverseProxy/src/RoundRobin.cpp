#include "../include/RoundRobin.h"

RoundRobin::RoundRobin()
    : index(0)
{
}

BackendServer* RoundRobin::nextServer(
    ServerPool& pool
)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<BackendServer>& servers =
        pool.getServers();

    if (servers.empty())
    {
        return nullptr;
    }

    int n =
        static_cast<int>(servers.size());

    for (int i = 0; i < n; i++)
    {
        BackendServer& server =
            servers[index];

        index =
            (index + 1) % n;

        if (server.healthy)
        {
            return &server;
        }
    }

    return nullptr;
}