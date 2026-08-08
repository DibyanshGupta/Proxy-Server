#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include "LoadBalancer.h"

class RoundRobin : public LoadBalancer
{
private:

    int index;

public:

    RoundRobin();

    BackendServer* nextServer(ServerPool &pool) override;
};

#endif