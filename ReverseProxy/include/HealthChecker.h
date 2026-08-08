#ifndef HEALTHCHECKER_H
#define HEALTHCHECKER_H

#include <thread>
#include <atomic>

#include "ServerPool.h"

class HealthChecker
{
private:

    ServerPool &pool;

    std::thread worker;

    std::atomic<bool> running;

    void checkLoop();

public:

    HealthChecker(ServerPool &p);

    void start();

    void stop();

    ~HealthChecker();
};

#endif