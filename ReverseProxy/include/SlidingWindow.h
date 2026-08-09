#ifndef SLIDINGWINDOW_H
#define SLIDINGWINDOW_H

#include "RateLimiter.h"

#include <chrono>
#include <deque>
#include <mutex>

class SlidingWindow : public RateLimiter
{
private:

    int limit;

    int window;

    std::deque<std::chrono::steady_clock::time_point> requests;

    std::mutex mtx;

public:

    SlidingWindow(int l, int w);

    bool allow() override;
};

#endif