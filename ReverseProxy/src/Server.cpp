#include "../include/Server.h"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Server::Server(int p, TaskQueue &q)
    : port(p), queue(q)
{
    sock = -1;
}

bool Server::start()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        std::cout << "Socket creation failed\n";
        return false;
    }

    int opt = 1;

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "Bind failed\n";
        return false;
    }

    if (listen(sock, 10) < 0)
    {
        std::cout << "Listen failed\n";
        return false;
    }

    std::cout << "Product Service started\n";
    std::cout << "Listening on port " << port << std::endl;

    return true;
}

void Server::run()
{
    while (true)
    {
        sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);

        int client = accept(sock, (sockaddr *)&clientAddr, &len);

        if (client < 0)
        {
            std::cout << "Accept failed\n";
            continue;
        }

        std::cout << "Client Connected" << std::endl;

        queue.push(client);
    }
}