#ifndef TOKENBUCKET_H
#define TOKENBUCKET_H

#include "RateLimiter.h"

#include <chrono>
#include <mutex>

class TokenBucket : public RateLimiter
{
private:

    double tokens;

    double capacity;

    double rate;

    std::chrono::steady_clock::time_point last;

    std::mutex mtx;

public:

    TokenBucket(double cap, double r);

    bool allow() override;
};

#endif