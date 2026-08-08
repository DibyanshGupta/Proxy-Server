#include "../include/Router.h"

Router::Router(LoadBalancer *pLB,
               LoadBalancer *uLB)
{
    productLB = pLB;
    userLB = uLB;
}

BackendServer* Router::getServer(const std::string &path)
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

ServerPool& Router::getProductPool()
{
    return productPool;
}

ServerPool& Router::getUserPool()
{
    return userPool;
}