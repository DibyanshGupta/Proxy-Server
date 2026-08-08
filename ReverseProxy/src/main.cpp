#include "../include/TaskQueue.h"
#include "../include/ThreadPool.h"
#include "../include/Server.h"

#include "../include/Router.h"

#include "../include/RoundRobin.h"
#include "../include/LeastConnections.h"

#include <thread>

int main()
{
    TaskQueue queue;

    RoundRobin productLB;

    LeastConnections userLB;

    Router router(&productLB, &userLB);

    router.getProductPool().addServer("127.0.0.1", 9003, 1);

    router.getUserPool().addServer("127.0.0.1", 9001, 1);

    int threads = std::thread::hardware_concurrency();

    if (threads == 0)
    {
        threads = 4;
    }

    ThreadPool pool(threads, queue, router);

    Server server(8080, queue);

    if (!server.start())
    {
        return 1;
    }

    server.run();

    return 0;
}