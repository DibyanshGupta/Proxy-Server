#include "../include/TaskQueue.h"
#include "../include/ThreadPool.h"
#include "../include/Server.h"

#include "../include/Router.h"

#include "../include/RoundRobin.h"
#include "../include/LeastConnections.h"
#include "../include/HealthChecker.h"

#include <thread>
#include <iostream>

int main()
{
    TaskQueue queue;


    // --------------------------------------------------
    // Load balancers
    // --------------------------------------------------

    RoundRobin productLB;

    LeastConnections userLB;


    // --------------------------------------------------
    // Router
    // --------------------------------------------------

    Router router(
        &productLB,
        &userLB
    );


    // --------------------------------------------------
    // Backend servers
    // --------------------------------------------------

    router.getProductPool().addServer(
        "127.0.0.1",
        9003,
        1
    );


    router.getUserPool().addServer(
        "127.0.0.1",
        9001,
        1
    );


    // --------------------------------------------------
    // Health checkers
    // --------------------------------------------------

    HealthChecker productHealthChecker(
        router.getProductPool()
    );

    HealthChecker userHealthChecker(
        router.getUserPool()
    );


    productHealthChecker.start();

    userHealthChecker.start();


    // --------------------------------------------------
    // Thread pool
    // --------------------------------------------------

    int threads =
        std::thread::hardware_concurrency();


    if (threads == 0)
    {
        threads = 4;
    }


    std::cout
        << "Starting "
        << threads
        << " worker threads..."
        << std::endl;


    ThreadPool pool(
        threads,
        queue,
        router
    );


    // --------------------------------------------------
    // Reverse Proxy server
    // --------------------------------------------------

    Server server(
        8080,
        queue
    );


    if (!server.start())
    {
        std::cerr
            << "Failed to start Reverse Proxy."
            << std::endl;

        return 1;
    }


    std::cout
        << "Reverse Proxy started on port 8080"
        << std::endl;


    // --------------------------------------------------
    // Run server
    // --------------------------------------------------

    server.run();


    // --------------------------------------------------
    // Stop health checkers
    // --------------------------------------------------

    productHealthChecker.stop();

    userHealthChecker.stop();


    return 0;
}