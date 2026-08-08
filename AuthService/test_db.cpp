#include <iostream>
#include <pqxx/pqxx>

int main()
{
    try
    {
        pqxx::connection db(
            "host=localhost "
            "port=5432 "
            "dbname=proxy_auth "
            "user=proxy_app "
            "password=proxy_app_password"
        );

        std::cout << "Connected to PostgreSQL successfully!"
                  << std::endl;

        pqxx::work transaction(db);

        pqxx::result result =
            transaction.exec("SELECT id, username, role FROM users");

        std::cout << "\nUsers in database:\n";

        for (const auto& row : result)
        {
            std::cout
                << "ID: " << row["id"].as<int>()
                << ", Username: " << row["username"].c_str()
                << ", Role: " << row["role"].c_str()
                << std::endl;
        }

        transaction.commit();

        std::cout << "\nDatabase query successful!"
                  << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Database error: "
            << e.what()
            << std::endl;

        return 1;
    }

    return 0;
}