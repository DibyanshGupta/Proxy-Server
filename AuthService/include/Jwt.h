#ifndef JWT_H
#define JWT_H

#include <string>

class Jwt
{
public:
    static std::string createToken(
        const std::string& username,
        const std::string& role
    );
};

#endif