#ifndef LEASTRESPONSETIME_H
#define LEASTRESPONSETIME_H

#include <mutex>

#include "LoadBalancer.h"

class LeastResponseTime : public LoadBalancer
{
private:

    std::mutex mtx;

public:

    BackendServer* nextServer(ServerPool &pool) override;

    void requestFinished(BackendServer *server,
                         double responseTime) override;
};

#endif