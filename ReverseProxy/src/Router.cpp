#include "../include/Router.h"

Router::Router(
    LoadBalancer* pLB,
    LoadBalancer* uLB
)
{
    productLB = pLB;
    userLB = uLB;
}


BackendServer* Router::getServer(
    const std::string& path
)
{
    if (path.find("/product") == 0)
    {
        return productLB->nextServer(productPool);
    }

    if (path.find("/user") == 0)
    {
        return userLB->nextServer(userPool);
    }

    return nullptr;
}


void Router::requestStarted(
    const std::string& path,
    BackendServer* server
)
{
    if (server == nullptr)
    {
        return;
    }

    if (path.find("/product") == 0)
    {
        productLB->requestStarted(server);
    }
    else if (path.find("/user") == 0)
    {
        userLB->requestStarted(server);
    }
}


void Router::requestFinished(
    const std::string& path,
    BackendServer* server,
    double responseTime
)
{
    if (server == nullptr)
    {
        return;
    }

    if (path.find("/product") == 0)
    {
        productLB->requestFinished(
            server,
            responseTime
        );
    }
    else if (path.find("/user") == 0)
    {
        userLB->requestFinished(
            server,
            responseTime
        );
    }
}


ServerPool& Router::getProductPool()
{
    return productPool;
}


ServerPool& Router::getUserPool()
{
    return userPool;
}