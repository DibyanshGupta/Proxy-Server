#ifndef ROUTER_H
#define ROUTER_H

#include <string>

#include "ServerPool.h"
#include "LoadBalancer.h"
#include "PerUserRateLimiter.h"

class Router
{
private:

    ServerPool productPool;
    ServerPool userPool;

    LoadBalancer* productLB;
    LoadBalancer* userLB;

    PerUserRateLimiter* rateLimiter;

public:

    Router(
        LoadBalancer* pLB,
        LoadBalancer* uLB,
        PerUserRateLimiter* limiter
    );

    BackendServer* getServer(
        const std::string& path
    );

    void requestStarted(
        const std::string& path,
        BackendServer* server
    );

    void requestFinished(
        const std::string& path,
        BackendServer* server,
        double responseTime
    );

    bool allowRequest(
        const std::string& username
    );

    ServerPool& getProductPool();

    ServerPool& getUserPool();
};

#endif