#include "../include/LeastConnections.h"

#include <climits>

BackendServer* LeastConnections::nextServer(
    ServerPool& pool
)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<BackendServer>& servers =
        pool.getServers();

    BackendServer* selected = nullptr;

    int minimumConnections = INT_MAX;

    for (auto& server : servers)
    {
        if (!server.healthy)
        {
            continue;
        }

        if (server.activeConnections <
            minimumConnections)
        {
            minimumConnections =
                server.activeConnections;

            selected = &server;
        }
    }

    return selected;
}


void LeastConnections::requestStarted(
    BackendServer* server
)
{
    if (server == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);

    server->activeConnections++;
}


void LeastConnections::requestFinished(
    BackendServer* server,
    double responseTime
)
{
    if (server == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);

    if (server->activeConnections > 0)
    {
        server->activeConnections--;
    }
}