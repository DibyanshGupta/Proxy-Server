#include "../include/LeastConnections.h"

BackendServer* LeastConnections::nextServer(ServerPool &pool)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<BackendServer> &servers = pool.getServers();

    BackendServer *best = nullptr;

    for (auto &server : servers)
    {
        if (!server.healthy)
        {
            continue;
        }

        if (best == nullptr ||
            server.activeConnections < best->activeConnections)
        {
            best = &server;
        }
    }

    return best;
}

void LeastConnections::requestStarted(BackendServer *server)
{
    if (server)
    {
        server->activeConnections++;
    }
}

void LeastConnections::requestFinished(BackendServer *server,
                                       double)
{
    if (server)
    {
        server->activeConnections--;
    }
}