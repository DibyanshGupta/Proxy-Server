#include "../include/Server.h"

#include <iostream>
#include <cstring>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

Server::Server(int port, ThreadPool& pool)
    : port(port), serverSocket(-1), pool(pool)
{
}

void Server::start()
{
    // Create socket
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (serverSocket < 0)
    {
        cerr << "Failed to create socket." << endl;
        return;
    }

    // Allow quick restart of the server
    int option = 1;

    if (setsockopt(
            serverSocket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &option,
            sizeof(option)) < 0)
    {
        cerr << "Warning: setsockopt failed." << endl;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;

    serverAddress.sin_port = htons(port);

    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (::bind(
            serverSocket,
            (sockaddr*)&serverAddress,
            sizeof(serverAddress)) < 0)
    {
        cerr << "Bind failed on port "
             << port << endl;

        close(serverSocket);

        return;
    }

    // Listen
    if (listen(serverSocket, 10) < 0)
    {
        cerr << "Listen failed." << endl;

        close(serverSocket);

        return;
    }

    cout << "======================================" << endl;
    cout << " Authentication Service" << endl;
    cout << " Listening on port " << port << endl;
    cout << "======================================" << endl;

    // Accept clients
    while (true)
    {
        sockaddr_in clientAddress{};

        socklen_t clientSize =
            sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddress,
            &clientSize
        );

        if (clientSocket < 0)
        {
            cerr << "Failed to accept client." << endl;
            continue;
        }

        cout << "New client accepted." << endl;

        pool.enqueue(clientSocket);
    }

    close(serverSocket);
}