#include "../include/WeightedRoundRobin.h"

BackendServer* WeightedRoundRobin::nextServer(ServerPool &pool)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<BackendServer> &servers = pool.getServers();

    if (servers.empty())
    {
        return nullptr;
    }

    BackendServer *best = nullptr;

    int totalWeight = 0;

    for (auto &server : servers)
    {
        if (!server.healthy)
        {
            continue;
        }

        totalWeight += server.weight;

        server.currentWeight += server.weight;

        if (best == nullptr ||
            server.currentWeight > best->currentWeight)
        {
            best = &server;
        }
    }

    if (best == nullptr)
    {
        return nullptr;
    }

    best->currentWeight -= totalWeight;

    return best;
}