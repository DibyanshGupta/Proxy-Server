#include <iostream>

#include "include/Database.h"
#include "include/PasswordHasher.h"

using namespace std;

int main()
{
    try
    {
        Database db(
            "host=localhost "
            "port=5432 "
            "dbname=proxy_auth "
            "user=proxy_app "
            "password=proxy_app_password"
        );

        string username = "testuser";

        string password = "hello123";

        cout << "Hashing password..." << endl;

        string hash =
            PasswordHasher::hashPassword(password);

        cout << "Generated hash:" << endl;
        cout << hash << endl;

        cout << "\nVerifying password..." << endl;

        bool valid =
            PasswordHasher::verifyPassword(
                password,
                hash
            );

        cout << "Correct password: "
             << (valid ? "YES" : "NO")
             << endl;

        cout << "\nTesting database..." << endl;

        if (db.userExists(username))
        {
            cout << "User already exists." << endl;
        }
        else
        {
            bool created =
                db.createUser(
                    username,
                    hash,
                    "user"
                );

            cout << "User created: "
                 << (created ? "YES" : "NO")
                 << endl;
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: "
             << e.what()
             << endl;

        return 1;
    }

    return 0;
}