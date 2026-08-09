#include "../include/SlidingWindow.h"

SlidingWindow::SlidingWindow(int l, int w)
{
    limit = l;
    window = w;
}

bool SlidingWindow::allow()
{
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::steady_clock::now();

    while (!requests.empty())
    {
        auto age =
            std::chrono::duration_cast<std::chrono::seconds>(
                now - requests.front()
            ).count();

        if (age >= window)
        {
            requests.pop_front();
        }
        else
        {
            break;
        }
    }

    if (requests.size() >= limit)
    {
        return false;
    }

    requests.push_back(now);

    return true;
}