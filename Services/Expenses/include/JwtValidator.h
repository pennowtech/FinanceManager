#pragma once

#include <jwt-cpp/jwt.h>

#include <string>

bool validateJwt(const std::string &token, const std::string &publicKey) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::rs256(
                                publicKey))  // This logic is explained above on how to get public key
                            .with_issuer("<https://securetoken.google.com/your-firebase-project-id>");

        verifier.verify(decoded);
        return true;
    } catch (...) {
        return false;
    }
}
