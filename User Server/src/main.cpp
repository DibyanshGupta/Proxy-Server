#include "../include/ThreadPool.h"
#include "../include/Server.h"

int main()
{
    ThreadPool pool(4);

    Server server(9001,pool);

    server.start();

    return 0;
}