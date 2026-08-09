#ifndef JWTVERIFIER_H
#define JWTVERIFIER_H

#include <string>

class JwtVerifier
{
public:

    static bool verifyToken(
        const std::string& token,
        std::string& username,
        std::string& role
    );

private:

    static std::string base64UrlDecode(
        const std::string& input
    );

    static std::string hmacSha256(
        const std::string& message,
        const std::string& secret
    );

    static bool constantTimeCompare(
        const std::string& a,
        const std::string& b
    );

    static bool extractJsonString(
        const std::string& json,
        const std::string& key,
        std::string& value
    );

    static bool extractJsonNumber(
        const std::string& json,
        const std::string& key,
        long long& value
    );
};

#endif