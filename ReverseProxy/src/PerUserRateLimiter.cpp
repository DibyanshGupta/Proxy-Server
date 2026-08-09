#include "../include/PerUserRateLimiter.h"

PerUserRateLimiter::PerUserRateLimiter(
    int requestLimit,
    int windowSeconds
)
    : limit(requestLimit),
      window(windowSeconds)
{
}

bool PerUserRateLimiter::allow(
    const std::string& username
)
{
    if (username.empty())
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx);

    auto it = userLimiters.find(username);

    if (it == userLimiters.end())
    {
        auto limiter =
            std::make_unique<SlidingWindow>(
                limit,
                window
            );

        it =
            userLimiters.emplace(
                username,
                std::move(limiter)
            ).first;
    }

    return it->second->allow();
}