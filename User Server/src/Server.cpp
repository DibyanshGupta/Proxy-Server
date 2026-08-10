#include "../include/Server.h"

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

using namespace std;

Server::Server(int port, ThreadPool& pool)
    : port(port), pool(pool)
{
}

void Server::start()
{
    // 1. Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        return;
    }

    // Allow immediate reuse of the port
    int opt = 1;

    if (setsockopt(serverSocket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(serverSocket);
        return;
    }

    // 2. Prepare server address
    sockaddr_in serverAddress{};
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind socket to port
    if (::bind(serverSocket,
             (sockaddr*)&serverAddress,
             sizeof(serverAddress)) < 0)
    {
        perror("bind");
        close(serverSocket);
        return;
    }

    // 4. Start listening
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        close(serverSocket);
        return;
    }

    cout << "User Service running on port "
         << port << endl;

    // 5. Accept clients
    while (true)
    {
        sockaddr_in clientAddress{};
        socklen_t clientSize = sizeof(clientAddress);

        int clientSocket =
            accept(serverSocket,
                   (sockaddr*)&clientAddress,
                   &clientSize);

        if (clientSocket < 0)
        {
            perror("accept");
            continue;
        }

        pool.enqueue(clientSocket);
    }
}