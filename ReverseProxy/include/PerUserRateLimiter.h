#ifndef PERUSERRATELIMITER_H
#define PERUSERRATELIMITER_H

#include "SlidingWindow.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class PerUserRateLimiter
{
private:

    int limit;
    int window;

    std::mutex mtx;

    std::unordered_map<
        std::string,
        std::unique_ptr<SlidingWindow>
    > userLimiters;

public:

    PerUserRateLimiter(
        int requestLimit,
        int windowSeconds
    );

    bool allow(
        const std::string& username
    );
};

#endif