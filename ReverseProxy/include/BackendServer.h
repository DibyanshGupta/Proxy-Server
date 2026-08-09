#ifndef BACKENDSERVER_H
#define BACKENDSERVER_H

#include <string>
#include <atomic>

class BackendServer
{
public:

    std::string host;

    int port;

    int weight;

    std::atomic<bool> healthy;

    int activeConnections;

    double avgResponseTime;

    int currentWeight;


    BackendServer(
        std::string h,
        int p,
        int w = 1
    )
        : host(std::move(h)),
          port(p),
          weight(w),
          healthy(true),
          activeConnections(0),
          avgResponseTime(0.0),
          currentWeight(0)
    {
    }


    // std::atomic is not copyable by default.
    // These functions allow BackendServer to remain
    // usable inside std::vector.

    BackendServer(
        const BackendServer& other
    )
        : host(other.host),
          port(other.port),
          weight(other.weight),
          healthy(other.healthy.load()),
          activeConnections(other.activeConnections),
          avgResponseTime(other.avgResponseTime),
          currentWeight(other.currentWeight)
    {
    }


    BackendServer& operator=(
        const BackendServer& other
    )
    {
        if (this != &other)
        {
            host = other.host;
            port = other.port;
            weight = other.weight;

            healthy.store(
                other.healthy.load()
            );

            activeConnections =
                other.activeConnections;

            avgResponseTime =
                other.avgResponseTime;

            currentWeight =
                other.currentWeight;
        }

        return *this;
    }
};

#endif