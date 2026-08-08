#include "../include/ThreadPool.h"
#include "../include/Server.h"
#include "../include/Database.h"

int main()
{
    // PostgreSQL connection
    Database database(
        "host=localhost "
        "port=5432 "
        "dbname=proxy_auth "
        "user=proxy_app "
        "password=proxy_app_password"
    );


    // Create thread pool
    ThreadPool pool(
        4,
        database
    );


    // Create authentication server
    Server server(
        9002,
        pool
    );


    server.start();


    return 0;
}