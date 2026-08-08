#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "BackendServer.h"
#include "ServerPool.h"
class LoadBalancer
{
public:


    virtual BackendServer* nextServer(ServerPool &pool)=0;

    virtual void requestStarted(BackendServer* server)
    {
    }

    virtual void requestFinished(BackendServer* server,double responseTime)
    {
    }

    virtual ~LoadBalancer() = default;
};

#endif