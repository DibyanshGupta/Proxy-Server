#ifndef SERVER_H
#define SERVER_H

#include "TaskQueue.h"

class Server
{
private:
    int sock;
    int port;
    TaskQueue &queue;

public:
    Server(int p, TaskQueue &q);

    bool start();

    void run();
};

#endif