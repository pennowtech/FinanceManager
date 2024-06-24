#pragma once

#include <drogon/HttpFilter.h>
#include <jwt-cpp/jwt.h>

class JwtAuthFilter : public drogon::HttpFilter<JwtAuthFilter> {
   private:
    std::string projectId;
    std::unordered_map<std::string, std::string> publicKeysMap;

   public:
    JwtAuthFilter(const std::string &projectId, const std::string &publicKeyUrl)
        : projectId(projectId), publicKeyUrl(publicKeyUrl) {}

    void doFilter(const drogon::HttpRequestPtr &req, drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&fccb) override {
        const std::string &authorizationHeader = req->getHeader("Authorization");
        if (authorizationHeader.find("Bearer ") != std::string::npos) {
            std::string token = authorizationHeader.substr(7);  // Strip 'Bearer ' to get the token

            try {
                // Verify the JWT token
                auto decodedToken = jwt::decode(token);
                auto kid = decodedToken.get_header_claim("kid").as_string();

                // Get the public key corresponding to the 'kid' in the JWT header
                //TODO: Handle scenario if key is not found.
                std::string publicKey = publicKeysMap.at(kid);

                auto verifier = jwt::verify()
                                    .allow_algorithm(jwt::algorithm::rs256{publicKey})
                                    .with_issuer("https://securetoken.google.com/" + projectId);

                verifier.verify(decodedToken);

                // Token is valid; continue the request
                fccb();
            } catch (const jwt::token_verification_exception &e) {
                // Token is invalid
                auto response = drogon::HttpResponse::newHttpResponse();
                response->setStatusCode(drogon::k401Unauthorized);
                response->setBody("Unauthorized: Invalid token");
                fcb(response);
                return;
            }
        } else {
            // No or improper Authorization header
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k401Unauthorized);
            response->setBody("Unauthorized: No token provided");
            fcb(response);
            return;
        }
    }
};
