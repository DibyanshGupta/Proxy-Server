#include "../include/HealthChecker.h"

#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

HealthChecker::HealthChecker(ServerPool &p)
    : pool(p), running(false)
{
}

void HealthChecker::start()
{
    running = true;

    worker = std::thread(&HealthChecker::checkLoop, this);
}

void HealthChecker::stop()
{
    running = false;

    if (worker.joinable())
    {
        worker.join();
    }
}

HealthChecker::~HealthChecker()
{
    stop();
}

void HealthChecker::checkLoop()
{
    while (running)
    {
        auto &servers = pool.getServers();

        for (auto &server : servers)
        {
            int sock = socket(AF_INET, SOCK_STREAM, 0);

            sockaddr_in addr;

            addr.sin_family = AF_INET;
            addr.sin_port = htons(server.port);

            inet_pton(AF_INET,
                      server.host.c_str(),
                      &addr.sin_addr);

            if (connect(sock,
                        (sockaddr *)&addr,
                        sizeof(addr)) == 0)
            {
                server.healthy = true;
            }
            else
            {
                server.healthy = false;
            }

            close(sock);
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(5));
    }
}