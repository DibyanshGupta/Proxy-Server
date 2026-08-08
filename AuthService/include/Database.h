#ifndef DATABASE_H
#define DATABASE_H

#include <string>

class Database
{
private:
    std::string connectionString;

public:
    Database(
        const std::string& connectionString
    );

    bool userExists(
        const std::string& username
    );

    bool createUser(
        const std::string& username,
        const std::string& passwordHash,
        const std::string& role
    );

    std::string getPasswordHash(
        const std::string& username
    );

    std::string getUserRole(
        const std::string& username
    );
};

#endif