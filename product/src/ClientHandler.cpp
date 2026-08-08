#include "../include/ClientHandler.h"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

void handleClient(int client)
{
    char buffer[4096];

    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
    {
        close(client);
        return;
    }

    std::string req(buffer);

    std::cout << "------------------------" << std::endl;
    std::cout << "Request Received\n";
    std::cout << req << std::endl;

    std::string body;
    std::string response;

    if (req.find("GET /product") != std::string::npos)
    {
        body =
            "{"
            "\"service\":\"Product Service\","
            "\"product\":\"Laptop\","
            "\"price\":65000"
            "}";

        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" +
            body;
    }
    else
    {
        body = "Not Found";

        response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" +
            body;
    }

    send(client, response.c_str(), response.size(), 0);

    close(client);
}