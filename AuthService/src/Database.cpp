#include "../include/Database.h"

#include <pqxx/pqxx>

using namespace std;

Database::Database(
    const string& connectionString
)
    : connectionString(connectionString)
{
}


bool Database::userExists(
    const string& username
)
{
    pqxx::connection db(connectionString);

    pqxx::work transaction(db);

    pqxx::result result =
        transaction.exec_params(
            "SELECT id FROM users "
            "WHERE username = $1",
            username
        );

    transaction.commit();

    return !result.empty();
}


bool Database::createUser(
    const string& username,
    const string& passwordHash,
    const string& role
)
{
    pqxx::connection db(connectionString);

    pqxx::work transaction(db);

    pqxx::result result =
        transaction.exec_params(
            "INSERT INTO users "
            "(username, password_hash, role) "
            "VALUES ($1, $2, $3) "
            "RETURNING id",
            username,
            passwordHash,
            role
        );

    transaction.commit();

    return !result.empty();
}


string Database::getPasswordHash(
    const string& username
)
{
    pqxx::connection db(connectionString);

    pqxx::work transaction(db);

    pqxx::result result =
        transaction.exec_params(
            "SELECT password_hash "
            "FROM users "
            "WHERE username = $1",
            username
        );

    transaction.commit();

    if (result.empty())
    {
        return "";
    }

    return result[0]["password_hash"].c_str();
}


string Database::getUserRole(
    const string& username
)
{
    pqxx::connection db(connectionString);

    pqxx::work transaction(db);

    pqxx::result result =
        transaction.exec_params(
            "SELECT role "
            "FROM users "
            "WHERE username = $1",
            username
        );

    transaction.commit();

    if (result.empty())
    {
        return "";
    }

    return result[0]["role"].c_str();
}