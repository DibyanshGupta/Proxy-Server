#include "../include/ServerPool.h"

void ServerPool::addServer(std::string host, int port, int weight)
{
    servers.push_back(BackendServer(host, port, weight));
}

std::vector<BackendServer>& ServerPool::getServers()
{
    return servers;
}