#include "../include/TokenBucket.h"

TokenBucket::TokenBucket(double cap, double r)
{
    capacity = cap;
    rate = r;

    tokens = capacity;

    last = std::chrono::steady_clock::now();
}

bool TokenBucket::allow()
{
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration<double>(
            now - last
        ).count();

    tokens += seconds * rate;

    if (tokens > capacity)
    {
        tokens = capacity;
    }

    last = now;

    if (tokens < 1)
    {
        return false;
    }

    tokens--;

    return true;
}