#ifndef ROUTER_H
#define ROUTER_H

#include <string>

#include "ServerPool.h"
#include "LoadBalancer.h"

class Router
{
private:
    ServerPool productPool;
    ServerPool userPool;

    LoadBalancer* productLB;
    LoadBalancer* userLB;

public:
    Router(
        LoadBalancer* pLB,
        LoadBalancer* uLB
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

    ServerPool& getProductPool();

    ServerPool& getUserPool();
};

#endif