
#include <JwtAuthFilter.h>
#include <drogon/HttpFilter.h>
#include <jwt-cpp/jwt.h>

#include <nlohmann/json.hpp>

JwtAuthFilter::JwtAuthFilter(const std::string& projectId, const std::unordered_map<std::string, std::string>& publicKeyMap)
    : m_projectId(projectId)
    , m_publicKeysMap(publicKeyMap)
{
}

void JwtAuthFilter::doFilter(const drogon::HttpRequestPtr& req, drogon::FilterCallback&& fcb, drogon::FilterChainCallback&& fccb)
{
    const std::string& authorizationHeader = req->getHeader("Authorization");
    if (authorizationHeader.find("Bearer ") != std::string::npos)
    {
        const std::string token = authorizationHeader.substr(7); // Strip 'Bearer ' to get the token

        try
        {
            // Verify the JWT token
            auto decodedToken = jwt::decode<jwt::default_traits>(token);

            std::cout << "SDSINGH. Printing JWT Keys..\n";

            for (auto& e : decodedToken.get_payload_json())
                std::cout << e.first << " = " << e.second << std::endl;

            auto kid = decodedToken.get_header_claim("kid").as_string();

            if (!decodedToken.has_header_claim("kid"))
            {
                // TODO: Handle missing 'kid'
            }
            if (m_publicKeysMap.find(kid) == m_publicKeysMap.end())
            {
                // TODO: Handle missing public key for 'kid'
            }

            // Get the public key corresponding to the 'kid' in the JWT header
            std::string publicKey = m_publicKeysMap.at(kid);

            auto verifier =
                jwt::verify().allow_algorithm(jwt::algorithm::rs256{publicKey}).with_issuer("https://securetoken.google.com/" + m_projectId);

            verifier.verify(decodedToken);

            // Token is valid; continue the request
            fccb();
        }
        catch (const jwt::token_expired_exception& e)
        {
            // Handle expired token
        }
        catch (const jwt::invalid_token_exception& e)
        {
            // Handle other token validation errors (like wrong signature)
        }
        catch (const jwt::token_verification_exception& e)
        {
            // Token is invalid
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k401Unauthorized);
            response->setBody("Unauthorized: Invalid token");
            fcb(response);
            return;
        }
        catch (const std::exception& e)
        {
            // Handle other standard exceptions
        }
    }
    else
    {
        // No or improper Authorization header
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k401Unauthorized);
        response->setBody("Unauthorized: No token provided");
        fcb(response);
        return;
    }
}
