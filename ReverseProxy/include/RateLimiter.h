#ifndef RATELIMITER_H
#define RATELIMITER_H

class RateLimiter
{
public:

    virtual bool allow() = 0;

    virtual ~RateLimiter() {}
};

#endif