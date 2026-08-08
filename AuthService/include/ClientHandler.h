#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "Database.h"

class ClientHandler
{
private:
    Database& database;

public:
    ClientHandler(Database& database);

    void handleClient(int clientSocket);
};

#endif