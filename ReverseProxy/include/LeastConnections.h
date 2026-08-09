#ifndef LEASTCONNECTIONS_H
#define LEASTCONNECTIONS_H

#include "LoadBalancer.h"

#include <mutex>

class LeastConnections : public LoadBalancer
{
private:
    std::mutex mtx;

public:
    BackendServer* nextServer(
        ServerPool& pool
    ) override;

    void requestStarted(
        BackendServer* server
    ) override;

    void requestFinished(
        BackendServer* server,
        double responseTime
    ) override;
};

#endif