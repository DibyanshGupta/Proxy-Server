#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include "LoadBalancer.h"

#include <mutex>

class RoundRobin : public LoadBalancer
{
private:
    int index;

    std::mutex mtx;

public:
    RoundRobin();

    BackendServer* nextServer(
        ServerPool& pool
    ) override;
};

#endif