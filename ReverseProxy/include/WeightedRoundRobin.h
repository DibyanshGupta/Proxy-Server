#ifndef WEIGHTEDROUNDROBIN_H
#define WEIGHTEDROUNDROBIN_H

#include <mutex>

#include "LoadBalancer.h"

class WeightedRoundRobin : public LoadBalancer
{
private:

    std::mutex mtx;

public:

    BackendServer* nextServer(ServerPool &pool) override;
};

#endif