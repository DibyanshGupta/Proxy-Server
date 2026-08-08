#include "../include/ClientHandler.h"

#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/socket.h>

using namespace std;

void ClientHandler::handleClient(int clientSocket)
{
    cout << "\n======================================" << endl;
    cout << "New Client Connected" << endl;
    cout << "======================================" << endl;

    char buffer[1024];

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived < 0)
    {
        cout << "Error receiving request." << endl;
        close(clientSocket);
        return;
    }

    if (bytesReceived == 0)
    {
        cout << "Client disconnected before sending data." << endl;
        close(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0';

    string request(buffer);

    cout << "\n----- Request Received -----" << endl;
    cout << request << endl;

    string response;

    // Check requested endpoint
    if (request.find("GET /user") != string::npos)
    {
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{"
            "\"service\":\"User Service\","
            "\"status\":\"success\","
            "\"user\":\"John Doe\""
            "}";
    }
    else
    {
        response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{"
            "\"error\":\"Endpoint Not Found\""
            "}";
    }

    int bytesSent = send(
        clientSocket,
        response.c_str(),
        response.length(),
        0
    );

    if (bytesSent < 0)
    {
        cout << "Failed to send response." << endl;
    }
    else
    {
        cout << "Response sent successfully." << endl;
    }

    close(clientSocket);

    cout << "Client disconnected." << endl;
    cout << "Waiting for next client..." << endl;
    cout << "======================================\n" << endl;
}