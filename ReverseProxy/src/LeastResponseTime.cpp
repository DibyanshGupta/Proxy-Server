#include "../include/LeastResponseTime.h"

BackendServer* LeastResponseTime::nextServer(
    ServerPool& pool
)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<BackendServer>& servers =
        pool.getServers();

    BackendServer* best = nullptr;

    for (auto& server : servers)
    {
        if (!server.healthy)
        {
            continue;
        }

        if (
            best == nullptr ||
            server.avgResponseTime <
                best->avgResponseTime
        )
        {
            best = &server;
        }
    }

    return best;
}


void LeastResponseTime::requestFinished(
    BackendServer* server,
    double responseTime
)
{
    if (server == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);

    if (server->avgResponseTime == 0)
    {
        server->avgResponseTime =
            responseTime;
    }
    else
    {
        server->avgResponseTime =
            (0.8 * server->avgResponseTime) +
            (0.2 * responseTime);
    }
}