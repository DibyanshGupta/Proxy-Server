#include "../include/JwtVerifier.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>

#include <ctime>
#include <string>

using namespace std;

namespace
{
    // MUST match the secret used by AuthService.
    const string SECRET =
        "my-super-secret-key";
}


// --------------------------------------------------
// Base64URL decode
// --------------------------------------------------

string JwtVerifier::base64UrlDecode(
    const string& input
)
{
    if (input.empty())
    {
        return "";
    }

    string base64 = input;

    // Base64URL -> normal Base64
    for (char& c : base64)
    {
        if (c == '-')
        {
            c = '+';
        }
        else if (c == '_')
        {
            c = '/';
        }
    }

    // Add padding.
    while (base64.size() % 4 != 0)
    {
        base64.push_back('=');
    }

    // Allocate an actual output buffer.
    string decoded(
        (base64.size() / 4) * 3,
        '\0'
    );

    int decodedLength =
        EVP_DecodeBlock(
            reinterpret_cast<unsigned char*>(
                decoded.data()
            ),
            reinterpret_cast<const unsigned char*>(
                base64.data()
            ),
            static_cast<int>(
                base64.size()
            )
        );

    if (decodedLength < 0)
    {
        return "";
    }

    // EVP_DecodeBlock includes bytes represented
    // by '=' padding in its returned length.
    int padding = 0;

    if (!base64.empty() &&
        base64.back() == '=')
    {
        padding++;

        if (base64.size() >= 2 &&
            base64[base64.size() - 2] == '=')
        {
            padding++;
        }
    }

    decodedLength -= padding;

    if (decodedLength < 0)
    {
        return "";
    }

    decoded.resize(
        static_cast<size_t>(decodedLength)
    );

    return decoded;
}


// --------------------------------------------------
// HMAC-SHA256
// --------------------------------------------------

string JwtVerifier::hmacSha256(
    const string& message,
    const string& secret
)
{
    unsigned int digestLength = 0;

    unsigned char digest[EVP_MAX_MD_SIZE];

    unsigned char* result =
        HMAC(
            EVP_sha256(),

            secret.data(),
            static_cast<int>(
                secret.size()
            ),

            reinterpret_cast<const unsigned char*>(
                message.data()
            ),

            message.size(),

            digest,
            &digestLength
        );

    if (result == nullptr)
    {
        return "";
    }

    return string(
        reinterpret_cast<char*>(digest),
        digestLength
    );
}


// --------------------------------------------------
// Constant-time comparison
// --------------------------------------------------

bool JwtVerifier::constantTimeCompare(
    const string& a,
    const string& b
)
{
    if (a.size() != b.size())
    {
        return false;
    }

    if (a.empty())
    {
        return true;
    }

    return CRYPTO_memcmp(
        a.data(),
        b.data(),
        a.size()
    ) == 0;
}


// --------------------------------------------------
// Extract JSON string
// --------------------------------------------------

bool JwtVerifier::extractJsonString(
    const string& json,
    const string& key,
    string& value
)
{
    string searchKey =
        "\"" + key + "\"";

    size_t keyPosition =
        json.find(searchKey);

    if (keyPosition == string::npos)
    {
        return false;
    }

    size_t colonPosition =
        json.find(
            ':',
            keyPosition + searchKey.length()
        );

    if (colonPosition == string::npos)
    {
        return false;
    }

    size_t firstQuote =
        json.find(
            '"',
            colonPosition + 1
        );

    if (firstQuote == string::npos)
    {
        return false;
    }

    size_t secondQuote =
        json.find(
            '"',
            firstQuote + 1
        );

    if (secondQuote == string::npos)
    {
        return false;
    }

    value =
        json.substr(
            firstQuote + 1,
            secondQuote - firstQuote - 1
        );

    return true;
}


// --------------------------------------------------
// Extract JSON number
// --------------------------------------------------

bool JwtVerifier::extractJsonNumber(
    const string& json,
    const string& key,
    long long& value
)
{
    string searchKey =
        "\"" + key + "\"";

    size_t keyPosition =
        json.find(searchKey);

    if (keyPosition == string::npos)
    {
        return false;
    }

    size_t colonPosition =
        json.find(
            ':',
            keyPosition + searchKey.length()
        );

    if (colonPosition == string::npos)
    {
        return false;
    }

    size_t numberStart =
        colonPosition + 1;

    while (
        numberStart < json.size() &&
        (
            json[numberStart] == ' ' ||
            json[numberStart] == '\t' ||
            json[numberStart] == '\r' ||
            json[numberStart] == '\n'
        )
    )
    {
        numberStart++;
    }

    if (numberStart >= json.size())
    {
        return false;
    }

    try
    {
        size_t numberLength = 0;

        value =
            stoll(
                json.substr(numberStart),
                &numberLength
            );

        return numberLength > 0;
    }
    catch (...)
    {
        return false;
    }
}


// --------------------------------------------------
// Verify JWT
// --------------------------------------------------

bool JwtVerifier::verifyToken(
    const string& token,
    string& username,
    string& role
)
{
    username.clear();
    role.clear();


    // --------------------------------------------------
    // JWT format:
    //
    // header.payload.signature
    // --------------------------------------------------

    size_t firstDot =
        token.find('.');

    if (firstDot == string::npos)
    {
        return false;
    }

    size_t secondDot =
        token.find(
            '.',
            firstDot + 1
        );

    if (secondDot == string::npos)
    {
        return false;
    }

    // There must be exactly two dots.
    if (
        token.find(
            '.',
            secondDot + 1
        ) != string::npos
    )
    {
        return false;
    }


    string encodedHeader =
        token.substr(
            0,
            firstDot
        );

    string encodedPayload =
        token.substr(
            firstDot + 1,
            secondDot - firstDot - 1
        );

    string encodedSignature =
        token.substr(
            secondDot + 1
        );


    if (
        encodedHeader.empty() ||
        encodedPayload.empty() ||
        encodedSignature.empty()
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Decode header
    // --------------------------------------------------

    string header =
        base64UrlDecode(
            encodedHeader
        );

    if (header.empty())
    {
        return false;
    }


    // --------------------------------------------------
    // Check algorithm
    // --------------------------------------------------

    if (
        header.find(
            "\"alg\":\"HS256\""
        ) == string::npos
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Decode payload
    // --------------------------------------------------

    string payload =
        base64UrlDecode(
            encodedPayload
        );

    if (payload.empty())
    {
        return false;
    }


    // --------------------------------------------------
    // Calculate expected signature
    // --------------------------------------------------

    string signingInput =
        encodedHeader +
        "." +
        encodedPayload;


    string expectedSignature =
        hmacSha256(
            signingInput,
            SECRET
        );


    if (expectedSignature.empty())
    {
        return false;
    }


    // --------------------------------------------------
    // Decode received signature
    // --------------------------------------------------

    string receivedSignature =
        base64UrlDecode(
            encodedSignature
        );

    if (receivedSignature.empty())
    {
        return false;
    }


    // --------------------------------------------------
    // Compare signatures
    // --------------------------------------------------

    if (
        !constantTimeCompare(
            expectedSignature,
            receivedSignature
        )
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Extract username
    // --------------------------------------------------

    if (
        !extractJsonString(
            payload,
            "sub",
            username
        )
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Extract role
    // --------------------------------------------------

    if (
        !extractJsonString(
            payload,
            "role",
            role
        )
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Extract expiration
    // --------------------------------------------------

    long long expiry = 0;

    if (
        !extractJsonNumber(
            payload,
            "exp",
            expiry
        )
    )
    {
        return false;
    }


    // --------------------------------------------------
    // Check expiration
    // --------------------------------------------------

    long long now =
        static_cast<long long>(
            time(nullptr)
        );

    if (now >= expiry)
    {
        return false;
    }


    return true;
}