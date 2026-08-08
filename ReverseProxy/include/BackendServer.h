#ifndef BACKENDSERVER_H
#define BACKENDSERVER_H

#include <string>

class BackendServer
{
public:

    std::string host;

    int port;

    int weight;

    bool healthy;

    int activeConnections;

    double avgResponseTime;

    int currentWeight;

    BackendServer(std::string h,
                  int p,
                  int w = 1)
    {
        host = h;
        port = p;
        weight = w;

        healthy = true;
        activeConnections = 0;

        avgResponseTime = 0;
        currentWeight = 0;
    }
};

#endif