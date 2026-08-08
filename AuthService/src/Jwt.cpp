#include "../include/Jwt.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>

#include <ctime>
#include <string>

using namespace std;

namespace
{
    /*
        IMPORTANT:

        This is only for our learning project.

        Later we should move this secret
        into an environment variable/config file.
    */

    const string SECRET =
        "my-super-secret-key";

    const string BASE64_CHARS =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


    string base64Encode(
        const unsigned char* data,
        size_t length
    )
    {
        string result;

        int value = 0;

        int bits = -6;

        for (size_t i = 0; i < length; i++)
        {
            value =
                (value << 8) + data[i];

            bits += 8;

            while (bits >= 0)
            {
                result.push_back(
                    BASE64_CHARS[
                        (value >> bits) & 0x3F
                    ]
                );

                bits -= 6;
            }
        }

        if (bits > -6)
        {
            result.push_back(
                BASE64_CHARS[
                    ((value << 8)
                    >> (bits + 8)) & 0x3F
                ]
            );
        }

        while (result.size() % 4)
        {
            result.push_back('=');
        }

        return result;
    }


    string base64UrlEncode(
        const string& input
    )
    {
        string encoded =
            base64Encode(
                reinterpret_cast<
                    const unsigned char*
                >(input.data()),
                input.size()
            );

        for (char& c : encoded)
        {
            if (c == '+')
            {
                c = '-';
            }
            else if (c == '/')
            {
                c = '_';
            }
        }

        while (
            !encoded.empty()
            && encoded.back() == '='
        )
        {
            encoded.pop_back();
        }

        return encoded;
    }


    string hmacSha256(
        const string& message,
        const string& secret
    )
    {
        unsigned int digestLength = 0;

        unsigned char digest[EVP_MAX_MD_SIZE];

        HMAC(
            EVP_sha256(),

            secret.data(),

            secret.size(),

            reinterpret_cast<
                const unsigned char*
            >(message.data()),

            message.size(),

            digest,

            &digestLength
        );

        return string(
            reinterpret_cast<char*>(digest),
            digestLength
        );
    }
}


string Jwt::createToken(
    const string& username,
    const string& role
)
{
    time_t now = time(nullptr);

    // Token expires after one hour
    time_t expiry = now + 3600;


    // JWT Header
    string header =
        "{"
        "\"alg\":\"HS256\","
        "\"typ\":\"JWT\""
        "}";


    // JWT Payload
    string payload =
        "{"
        "\"sub\":\"" + username + "\","
        "\"role\":\"" + role + "\","
        "\"iat\":" + to_string(now) + ","
        "\"exp\":" + to_string(expiry) +
        "}";


    // Encode header
    string encodedHeader =
        base64UrlEncode(header);


    // Encode payload
    string encodedPayload =
        base64UrlEncode(payload);


    // Header.Payload
    string message =
        encodedHeader
        + "."
        + encodedPayload;


    // Create signature
    string signature =
        hmacSha256(
            message,
            SECRET
        );


    // Encode signature
    string encodedSignature =
        base64UrlEncode(signature);


    // Final JWT
    return message
        + "."
        + encodedSignature;
}