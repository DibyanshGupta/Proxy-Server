#include "../include/ClientHandler.h"
#include "../include/JwtVerifier.h"

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;


// --------------------------------------------------
// Send a simple HTTP response
// --------------------------------------------------

static void sendHttpResponse(
    int client,
    const string& status,
    const string& body
)
{
    string response =
        "HTTP/1.1 " + status + "\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " +
        to_string(body.size()) +
        "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        body;

    send(
        client,
        response.c_str(),
        response.size(),
        0
    );
}


// --------------------------------------------------
// Extract Authorization header
//
// Expected:
//
// Authorization: Bearer eyJ...
// --------------------------------------------------

static string extractBearerToken(
    const string& request
)
{
    const string headerName =
        "Authorization:";

    size_t position =
        request.find(headerName);

    if (position == string::npos)
    {
        return "";
    }

    size_t valueStart =
        position + headerName.length();

    // Skip spaces/tabs
    while (
        valueStart < request.size() &&
        (
            request[valueStart] == ' ' ||
            request[valueStart] == '\t'
        )
    )
    {
        valueStart++;
    }

    const string bearerPrefix =
        "Bearer ";

    if (
        request.compare(
            valueStart,
            bearerPrefix.length(),
            bearerPrefix
        ) != 0
    )
    {
        return "";
    }

    valueStart += bearerPrefix.length();

    size_t valueEnd =
        request.find(
            "\r\n",
            valueStart
        );

    if (valueEnd == string::npos)
    {
        valueEnd = request.size();
    }

    string token =
        request.substr(
            valueStart,
            valueEnd - valueStart
        );

    // Remove accidental trailing spaces.
    while (
        !token.empty() &&
        (
            token.back() == ' ' ||
            token.back() == '\t'
        )
    )
    {
        token.pop_back();
    }

    return token;
}


// --------------------------------------------------
// Handle client
// --------------------------------------------------

void handleClient(
    int client,
    Router& router
)
{
    char buffer[8192];

    memset(
        buffer,
        0,
        sizeof(buffer)
    );


    // --------------------------------------------------
    // Receive request from client
    // --------------------------------------------------

    int bytesReceived =
        recv(
            client,
            buffer,
            sizeof(buffer) - 1,
            0
        );


    if (bytesReceived <= 0)
    {
        close(client);
        return;
    }


    buffer[bytesReceived] = '\0';


    string request(buffer);


    cout << "\n----- Incoming Request -----"
         << endl;

    cout << request << endl;


    // --------------------------------------------------
    // Extract HTTP request line
    // --------------------------------------------------

    size_t requestLineEnd =
        request.find("\r\n");


    if (requestLineEnd == string::npos)
    {
        sendHttpResponse(
            client,
            "400 Bad Request",
            "{\"error\":\"Invalid HTTP request\"}"
        );

        close(client);
        return;
    }


    string requestLine =
        request.substr(
            0,
            requestLineEnd
        );


    // Example:
    //
    // GET /user HTTP/1.1
    //
    // We want:
    //
    // /user

    size_t methodEnd =
        requestLine.find(' ');


    if (methodEnd == string::npos)
    {
        sendHttpResponse(
            client,
            "400 Bad Request",
            "{\"error\":\"Invalid request line\"}"
        );

        close(client);
        return;
    }


    string method =
        requestLine.substr(
            0,
            methodEnd
        );


    size_t pathStart =
        methodEnd + 1;


    size_t pathEnd =
        requestLine.find(
            ' ',
            pathStart
        );


    if (pathEnd == string::npos)
    {
        sendHttpResponse(
            client,
            "400 Bad Request",
            "{\"error\":\"Invalid request line\"}"
        );

        close(client);
        return;
    }


    string path =
        requestLine.substr(
            pathStart,
            pathEnd - pathStart
        );


    cout << "Requested path: "
         << path
         << endl;


    // --------------------------------------------------
    // Authentication
    //
    // AuthService is responsible for:
    //     registration
    //     login
    //     JWT creation
    //
    // Reverse Proxy is responsible for:
    //     JWT verification
    // --------------------------------------------------

    string token =
        extractBearerToken(request);


    if (token.empty())
    {
        cout << "Authentication failed: "
                "Bearer token missing."
             << endl;


        sendHttpResponse(
            client,
            "401 Unauthorized",
            "{\"error\":\"Authentication required\"}"
        );


        close(client);

        return;
    }


    string username;
    string role;


    bool authenticated =
        JwtVerifier::verifyToken(
            token,
            username,
            role
        );


    if (!authenticated)
    {
        cout << "Authentication failed: "
                "invalid or expired JWT."
             << endl;


        sendHttpResponse(
            client,
            "401 Unauthorized",
            "{\"error\":\"Invalid or expired token\"}"
        );


        close(client);

        return;
    }


    cout << "Authenticated user: "
         << username
         << endl;

    cout << "User role: "
         << role
         << endl;

    // --------------------------------------------------
// Rate limiting
// --------------------------------------------------

if (!router.allowRequest(username))
{
    cout << "Rate limit exceeded for user: "
         << username
         << endl;

    sendHttpResponse(
        client,
        "429 Too Many Requests",
        "{\"error\":\"Rate limit exceeded\"}"
    );

    close(client);

    return;
}

cout << "Rate limit check passed."
     << endl;

    // --------------------------------------------------
    // Select backend server
    // --------------------------------------------------

    BackendServer* server =
        router.getServer(path);


    if (server == nullptr)
    {
        sendHttpResponse(
            client,
            "503 Service Unavailable",
            "{\"error\":\"No backend available\"}"
        );


        close(client);

        return;
    }


    cout << "Selected backend: "
         << server->host
         << ":"
         << server->port
         << endl;


    // --------------------------------------------------
    // Tell load balancer request started
    // --------------------------------------------------

    router.requestStarted(
        path,
        server
    );


    // Start response-time measurement
    auto startTime =
        steady_clock::now();


    // --------------------------------------------------
    // Create backend socket
    // --------------------------------------------------

    int backendSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );


    if (backendSocket < 0)
    {
        cerr << "Failed to create backend socket."
             << endl;


        router.requestFinished(
            path,
            server,
            0.0
        );


        sendHttpResponse(
            client,
            "502 Bad Gateway",
            "{\"error\":\"Failed to create backend connection\"}"
        );


        close(client);

        return;
    }


    sockaddr_in backendAddress{};


    backendAddress.sin_family =
        AF_INET;

    backendAddress.sin_port =
        htons(server->port);


    if (
        inet_pton(
            AF_INET,
            server->host.c_str(),
            &backendAddress.sin_addr
        ) <= 0
    )
    {
        cerr << "Invalid backend address: "
             << server->host
             << endl;


        close(backendSocket);


        router.requestFinished(
            path,
            server,
            0.0
        );


        sendHttpResponse(
            client,
            "502 Bad Gateway",
            "{\"error\":\"Invalid backend address\"}"
        );


        close(client);

        return;
    }


    // --------------------------------------------------
    // Connect to backend
    // --------------------------------------------------

    if (
        connect(
            backendSocket,
            reinterpret_cast<sockaddr*>(
                &backendAddress
            ),
            sizeof(backendAddress)
        ) < 0
    )
    {
        cerr << "Failed to connect to backend "
             << server->host
             << ":"
             << server->port
             << endl;


        close(backendSocket);


        auto endTime =
            steady_clock::now();


        double responseTime =
            duration<double, milli>(
                endTime - startTime
            ).count();


        router.requestFinished(
            path,
            server,
            responseTime
        );


        sendHttpResponse(
            client,
            "502 Bad Gateway",
            "{\"error\":\"Backend unavailable\"}"
        );


        close(client);

        return;
    }


    // --------------------------------------------------
    // Forward request to backend
    // --------------------------------------------------

    size_t totalSent = 0;


    while (
        totalSent < request.size()
    )
    {
        ssize_t sent =
            send(
                backendSocket,
                request.data() + totalSent,
                request.size() - totalSent,
                0
            );


        if (sent <= 0)
        {
            cerr << "Failed to forward request."
                 << endl;


            close(backendSocket);


            auto endTime =
                steady_clock::now();


            double responseTime =
                duration<double, milli>(
                    endTime - startTime
                ).count();


            router.requestFinished(
                path,
                server,
                responseTime
            );


            sendHttpResponse(
                client,
                "502 Bad Gateway",
                "{\"error\":\"Failed to forward request\"}"
            );


            close(client);

            return;
        }


        totalSent +=
            static_cast<size_t>(sent);
    }


    // --------------------------------------------------
    // Receive backend response
    // --------------------------------------------------

    char responseBuffer[8192];


    bool backendError = false;


    while (true)
    {
        int received =
            recv(
                backendSocket,
                responseBuffer,
                sizeof(responseBuffer),
                0
            );


        if (received < 0)
        {
            cerr << "Error receiving backend response."
                 << endl;

            backendError = true;

            break;
        }


        if (received == 0)
        {
            break;
        }


        // --------------------------------------------------
        // Forward response to client
        // --------------------------------------------------

        size_t totalResponseSent = 0;


        while (
            totalResponseSent <
            static_cast<size_t>(received)
        )
        {
            ssize_t sent =
                send(
                    client,
                    responseBuffer +
                        totalResponseSent,
                    received -
                        totalResponseSent,
                    0
                );


            if (sent <= 0)
            {
                cerr << "Failed to send response "
                        "to client."
                     << endl;

                backendError = true;

                break;
            }


            totalResponseSent +=
                static_cast<size_t>(sent);
        }


        if (backendError)
        {
            break;
        }
    }


    // --------------------------------------------------
    // Calculate response time
    // --------------------------------------------------

    auto endTime =
        steady_clock::now();


    double responseTime =
        duration<double, milli>(
            endTime - startTime
        ).count();


    cout << "Backend response time: "
         << responseTime
         << " ms"
         << endl;


    // --------------------------------------------------
    // Tell load balancer request finished
    // --------------------------------------------------

    router.requestFinished(
        path,
        server,
        responseTime
    );


    // --------------------------------------------------
    // Cleanup
    // --------------------------------------------------

    close(backendSocket);

    close(client);


    cout << "Request completed."
         << endl;
}