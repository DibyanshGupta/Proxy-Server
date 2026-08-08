#include "../include/ClientHandler.h"
#include "../include/PasswordHasher.h"
#include "../include/Jwt.h"

#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/socket.h>

using namespace std;


// --------------------------------------------------
// Helper function
// Extracts a simple JSON string value.
//
// Example:
// {"username":"alice","password":"hello123"}
//
// extractJsonValue(body, "username")
// returns:
// alice
// --------------------------------------------------

static string extractJsonValue(
    const string& json,
    const string& key
)
{
    string searchKey = "\"" + key + "\"";

    size_t keyPosition =
        json.find(searchKey);

    if (keyPosition == string::npos)
    {
        return "";
    }

    size_t colonPosition =
        json.find(
            ':',
            keyPosition + searchKey.length()
        );

    if (colonPosition == string::npos)
    {
        return "";
    }

    size_t firstQuote =
        json.find(
            '"',
            colonPosition + 1
        );

    if (firstQuote == string::npos)
    {
        return "";
    }

    size_t secondQuote =
        json.find(
            '"',
            firstQuote + 1
        );

    if (secondQuote == string::npos)
    {
        return "";
    }

    return json.substr(
        firstQuote + 1,
        secondQuote - firstQuote - 1
    );
}


// --------------------------------------------------
// Constructor
// --------------------------------------------------

ClientHandler::ClientHandler(
    Database& database
)
    : database(database)
{
}


// --------------------------------------------------
// Handle client
// --------------------------------------------------

void ClientHandler::handleClient(
    int clientSocket
)
{
    cout << "\n======================================"
         << endl;

    cout << "New Authentication Request"
         << endl;

    cout << "======================================"
         << endl;


    char buffer[4096];


    int bytesReceived =
        recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );


    if (bytesReceived < 0)
    {
        cerr << "Error receiving request."
             << endl;

        close(clientSocket);

        return;
    }


    if (bytesReceived == 0)
    {
        cout << "Client disconnected."
             << endl;

        close(clientSocket);

        return;
    }


    buffer[bytesReceived] = '\0';


    string request(buffer);


    cout << "\n----- Request Received -----"
         << endl;

    cout << request << endl;


    // --------------------------------------------------
    // Separate HTTP headers and body
    // --------------------------------------------------

    size_t bodyPosition =
        request.find("\r\n\r\n");


    string body;


    if (bodyPosition != string::npos)
    {
        body =
            request.substr(
                bodyPosition + 4
            );
    }


    string response;


    try
    {
        // ==================================================
        // REGISTER
        // ==================================================

        if (request.rfind(
                "POST /register",
                0
            ) == 0)
        {
            cout << "Register endpoint requested."
                 << endl;


            string username =
                extractJsonValue(
                    body,
                    "username"
                );


            string password =
                extractJsonValue(
                    body,
                    "password"
                );


            // ----------------------------------------------
            // Validate input
            // ----------------------------------------------

            if (
                username.empty()
                ||
                password.empty()
            )
            {
                string responseBody =
                    "{"
                    "\"error\":\"Username and password are required\""
                    "}";


                response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: " +
                    to_string(
                        responseBody.length()
                    ) +
                    "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    responseBody;
            }

            // ----------------------------------------------
            // Check if user already exists
            // ----------------------------------------------

            else if (
                database.userExists(
                    username
                )
            )
            {
                cout << "Username already exists: "
                     << username
                     << endl;


                string responseBody =
                    "{"
                    "\"error\":\"Username already exists\""
                    "}";


                response =
                    "HTTP/1.1 409 Conflict\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: " +
                    to_string(
                        responseBody.length()
                    ) +
                    "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    responseBody;
            }

            // ----------------------------------------------
            // Create new user
            // ----------------------------------------------

            else
            {
                cout << "Creating user: "
                     << username
                     << endl;


                string passwordHash =
                    PasswordHasher::hashPassword(
                        password
                    );


                bool created =
                    database.createUser(
                        username,
                        passwordHash,
                        "user"
                    );


                if (created)
                {
                    string responseBody =
                        "{"
                        "\"message\":\"Registration successful\""
                        "}";


                    response =
                        "HTTP/1.1 201 Created\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        to_string(
                            responseBody.length()
                        ) +
                        "\r\n"
                        "Connection: close\r\n"
                        "\r\n" +
                        responseBody;
                }
                else
                {
                    string responseBody =
                        "{"
                        "\"error\":\"Registration failed\""
                        "}";


                    response =
                        "HTTP/1.1 500 Internal Server Error\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        to_string(
                            responseBody.length()
                        ) +
                        "\r\n"
                        "Connection: close\r\n"
                        "\r\n" +
                        responseBody;
                }
            }
        }


        // ==================================================
        // LOGIN
        // ==================================================

        else if (
            request.rfind(
                "POST /login",
                0
            ) == 0
        )
        {
            cout << "Login endpoint requested."
                 << endl;


            string username =
                extractJsonValue(
                    body,
                    "username"
                );


            string password =
                extractJsonValue(
                    body,
                    "password"
                );


            // ----------------------------------------------
            // Validate input
            // ----------------------------------------------

            if (
                username.empty()
                ||
                password.empty()
            )
            {
                string responseBody =
                    "{"
                    "\"error\":\"Username and password are required\""
                    "}";


                response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: " +
                    to_string(
                        responseBody.length()
                    ) +
                    "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    responseBody;
            }

            else
            {
                // ------------------------------------------
                // Get stored password hash
                // ------------------------------------------

                string storedHash =
                    database.getPasswordHash(
                        username
                    );


                if (storedHash.empty())
                {
                    cout << "User not found."
                         << endl;


                    string responseBody =
                        "{"
                        "\"error\":\"Invalid username or password\""
                        "}";


                    response =
                        "HTTP/1.1 401 Unauthorized\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        to_string(
                            responseBody.length()
                        ) +
                        "\r\n"
                        "Connection: close\r\n"
                        "\r\n" +
                        responseBody;
                }

                // ------------------------------------------
                // Verify password
                // ------------------------------------------

                else if (
                    !PasswordHasher::verifyPassword(
                        password,
                        storedHash
                    )
                )
                {
                    cout << "Invalid password."
                         << endl;


                    string responseBody =
                        "{"
                        "\"error\":\"Invalid username or password\""
                        "}";


                    response =
                        "HTTP/1.1 401 Unauthorized\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        to_string(
                            responseBody.length()
                        ) +
                        "\r\n"
                        "Connection: close\r\n"
                        "\r\n" +
                        responseBody;
                }

                // ------------------------------------------
                // Password correct → JWT
                // ------------------------------------------

                else
                {
                    cout << "Login successful for: "
                         << username
                         << endl;


                    string role =
                        database.getUserRole(
                            username
                        );


                    string token =
                        Jwt::createToken(
                            username,
                            role
                        );


                    string responseBody =
                        "{"
                        "\"message\":\"Login successful\","
                        "\"token\":\"" +
                        token +
                        "\""
                        "}";


                    response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        to_string(
                            responseBody.length()
                        ) +
                        "\r\n"
                        "Connection: close\r\n"
                        "\r\n" +
                        responseBody;
                }
            }
        }


        // ==================================================
        // UNKNOWN ENDPOINT
        // ==================================================

        else
        {
            cout << "Unknown endpoint."
                 << endl;


            string responseBody =
                "{"
                "\"error\":\"Endpoint Not Found\""
                "}";


            response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: " +
                to_string(
                    responseBody.length()
                ) +
                "\r\n"
                "Connection: close\r\n"
                "\r\n" +
                responseBody;
        }
    }

    catch (const exception& e)
    {
        cerr << "Authentication error: "
             << e.what()
             << endl;


        string responseBody =
            "{"
            "\"error\":\"Internal Server Error\""
            "}";


        response =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " +
            to_string(
                responseBody.length()
            ) +
            "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            responseBody;
    }


    // --------------------------------------------------
    // Send response
    // --------------------------------------------------

    int bytesSent =
        send(
            clientSocket,
            response.c_str(),
            response.length(),
            0
        );


    if (bytesSent < 0)
    {
        cerr << "Failed to send response."
             << endl;
    }
    else
    {
        cout << "Response sent successfully."
             << endl;
    }


    close(clientSocket);


    cout << "Client disconnected."
         << endl;
}