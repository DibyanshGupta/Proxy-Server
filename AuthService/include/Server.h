#ifndef SERVER_H
#define SERVER_H

#include "ThreadPool.h"

class Server
{
private:
    int port;

    int serverSocket;

    ThreadPool& pool;

public:
    Server(int port, ThreadPool& pool);

    void start();
};

#endif