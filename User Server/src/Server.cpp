#include "../include/Server.h"

#include <iostream>

#include <unistd.h>

#include <arpa/inet.h>

using namespace std;

Server::Server(int port, ThreadPool& pool)
    : port(port), pool(pool)
{

}

void Server::start()
{
    serverSocket = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,
         (sockaddr*)&serverAddress,
         sizeof(serverAddress));

    listen(serverSocket,5);

    cout<<"User Service running on port "<<port<<endl;

    while(true)
    {
        sockaddr_in clientAddress;

        socklen_t clientSize=sizeof(clientAddress);

        int clientSocket =
            accept(serverSocket,
                   (sockaddr*)&clientAddress,
                   &clientSize);

        if(clientSocket>=0)
        {
            pool.enqueue(clientSocket);
        }
    }
}