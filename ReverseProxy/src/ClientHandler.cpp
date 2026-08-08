#include "../include/ClientHandler.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void handleClient(int client, Router &router)
{
    char buffer[4096];

    int bytes = recv(client,
                     buffer,
                     sizeof(buffer),
                     0);

    if (bytes <= 0)
    {
        close(client);
        return;
    }

    std::string request(buffer, bytes);

    size_t pos = request.find("\r\n");

    std::string firstLine =
        request.substr(0, pos);

    std::stringstream ss(firstLine);

    std::string method;
    std::string path;
    std::string version;

    ss >> method >> path >> version;

    BackendServer *server =
        router.getServer(path);

    if (server == nullptr)
    {
        std::string body = "No Backend";

        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: " +
            std::to_string(body.size()) +
            "\r\n\r\n" +
            body;

        int total = 0;

        while (total < response.size())
        {
            int sent = send(client,
                            response.c_str() + total,
                            response.size() - total,
                            0);

            if (sent <= 0)
            {
                break;
            }

            total += sent;
        }

        close(client);
        return;
    }

    int backend = socket(AF_INET,
                         SOCK_STREAM,
                         0);

    if (backend < 0)
    {
        close(client);
        return;
    }

    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server->port);

    inet_pton(AF_INET,
              server->host.c_str(),
              &addr.sin_addr);

    if (connect(backend,
                (sockaddr *)&addr,
                sizeof(addr)) < 0)
    {
        close(backend);
        close(client);
        return;
    }

    int total = 0;

    while (total < request.size())
    {
        int sent = send(backend,
                        request.c_str() + total,
                        request.size() - total,
                        0);

        if (sent <= 0)
        {
            break;
        }

        total += sent;
    }

    while (true)
    {
        bytes = recv(backend,
                     buffer,
                     sizeof(buffer),
                     0);

        if (bytes <= 0)
        {
            break;
        }

        int sentTotal = 0;

        while (sentTotal < bytes)
        {
            int sent = send(client,
                            buffer + sentTotal,
                            bytes - sentTotal,
                            0);

            if (sent <= 0)
            {
                break;
            }

            sentTotal += sent;
        }

        if (sentTotal < bytes)
        {
            break;
        }
    }

    close(backend);

    close(client);
}