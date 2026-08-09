#include "../include/HealthChecker.h"

#include <arpa/inet.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


HealthChecker::HealthChecker(
    ServerPool& p
)
    : pool(p),
      running(false)
{
}


void HealthChecker::start()
{
    if (running)
    {
        return;
    }

    running = true;

    worker =
        std::thread(
            &HealthChecker::checkLoop,
            this
        );
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
        auto& servers =
            pool.getServers();


        for (auto& server : servers)
        {
            if (!running)
            {
                break;
            }


            int sock =
                socket(
                    AF_INET,
                    SOCK_STREAM,
                    0
                );


            if (sock < 0)
            {
                server.healthy = false;
                continue;
            }


            sockaddr_in addr{};

            addr.sin_family =
                AF_INET;

            addr.sin_port =
                htons(server.port);


            if (
                inet_pton(
                    AF_INET,
                    server.host.c_str(),
                    &addr.sin_addr
                ) <= 0
            )
            {
                server.healthy = false;

                close(sock);

                continue;
            }


            int result =
                connect(
                    sock,
                    reinterpret_cast<sockaddr*>(
                        &addr
                    ),
                    sizeof(addr)
                );


            if (result == 0)
            {
                server.healthy = true;

                std::cout
                    << "[HealthChecker] "
                    << server.host
                    << ":"
                    << server.port
                    << " HEALTHY"
                    << std::endl;
            }
            else
            {
                server.healthy = false;

                std::cout
                    << "[HealthChecker] "
                    << server.host
                    << ":"
                    << server.port
                    << " UNHEALTHY"
                    << std::endl;
            }


            close(sock);
        }


        // Check every 5 seconds.
        for (int i = 0; i < 50 && running; ++i)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }
    }
}