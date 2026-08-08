#include "../include/PasswordHasher.h"

#include <argon2.h>
#include <openssl/rand.h>

#include <stdexcept>
#include <vector>
#include <cstdint>

using namespace std;

string PasswordHasher::hashPassword(
    const string& password
)
{
    // Argon2id parameters
    const uint32_t timeCost = 3;
    const uint32_t memoryCost = 65536;
    const uint32_t parallelism = 1;

    const size_t saltLength = 16;
    const size_t hashLength = 32;

    // Generate a cryptographically secure random salt
    vector<unsigned char> salt(saltLength);

    if (RAND_bytes(
            salt.data(),
            static_cast<int>(saltLength)
        ) != 1)
    {
        throw runtime_error(
            "Failed to generate secure random salt"
        );
    }

    // Calculate required encoded hash size
    size_t encodedLength =
        argon2_encodedlen(
            timeCost,
            memoryCost,
            parallelism,
            saltLength,
            hashLength,
            Argon2_id
        );

    vector<char> encodedHash(encodedLength);

    // Generate Argon2id password hash
    int result =
        argon2id_hash_encoded(
            timeCost,
            memoryCost,
            parallelism,

            password.data(),
            password.size(),

            salt.data(),
            saltLength,

            hashLength,

            encodedHash.data(),
            encodedLength
        );

    if (result != ARGON2_OK)
    {
        throw runtime_error(
            string("Failed to hash password: ")
            + argon2_error_message(result)
        );
    }

    return string(encodedHash.data());
}


bool PasswordHasher::verifyPassword(
    const string& password,
    const string& hash
)
{
    int result =
        argon2id_verify(
            hash.c_str(),
            password.data(),
            password.size()
        );

    return result == ARGON2_OK;
}