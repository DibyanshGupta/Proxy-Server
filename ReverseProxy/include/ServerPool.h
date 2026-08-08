#ifndef SERVERPOOL_H
#define SERVERPOOL_H

#include <vector>
#include <string>

#include "BackendServer.h"

class ServerPool
{
private:

    std::vector<BackendServer> servers;

public:

    void addServer(std::string host, int port, int weight = 1);

    std::vector<BackendServer>& getServers();
};

#endif