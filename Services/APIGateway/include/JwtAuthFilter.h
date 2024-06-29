/*----------------------------------------------------------------------
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ----------------------------------------------------------------------*/
#pragma once

#include <drogon/HttpFilter.h>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>

class JwtAuthFilter : public drogon::HttpFilter<JwtAuthFilter>
{
private:
    std::string m_projectId;
    std::unordered_map<std::string, std::string> m_publicKeysMap;

public:
    JwtAuthFilter(const std::string& projectId, const std::unordered_map<std::string, std::string>& publicKeyMap)
        : m_projectId(projectId)
        , m_publicKeysMap(publicKeyMap)
    {
    }

    void doFilter(const drogon::HttpRequestPtr& req, drogon::FilterCallback&& fcb, drogon::FilterChainCallback&& fccb)
    {
        const std::string& authorizationHeader = req->getHeader("Authorization");
        if (authorizationHeader.find("Bearer ") != std::string::npos)
        {
            const std::string token = authorizationHeader.substr(7); // Strip 'Bearer ' to get the token

            try
            {
                // Verify the JWT token.
                auto decodedToken = jwt::decode<jwt::traits::nlohmann_json>(token);

                std::cout << "SDSINGH. Printing JWT Keys..\n";

                for (auto& e : decodedToken.get_payload_json())
                    std::cout << e.first << " = " << e.second << std::endl;

                auto kid = decodedToken.get_header_claim("kid").as_string();

                if (!decodedToken.has_header_claim("kid"))
                {
                    // TODO: Handle missing 'kid'.
                }
                if (m_publicKeysMap.find(kid) == m_publicKeysMap.end())
                {
                    // TODO: Handle missing public key for 'kid'
                }

                // Get the public key corresponding to the 'kid' in the JWT header
                std::string publicKey = m_publicKeysMap.at(kid);

                auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
                                    .allow_algorithm(jwt::algorithm::rs256{publicKey})
                                    .with_issuer("https://securetoken.google.com/" + m_projectId);

                verifier.verify(decodedToken);

                // Token is valid; continue the request
                fccb();
            }
            catch (const jwt::error::invalid_json_exception& e)
            {
                // Handle other token validation errors (like wrong signature)
                std::cout << "token validation errors (invalid_json_exception).\n";
            }
            catch (const jwt::error::token_verification_exception& e)
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
            // No or improper Authorization header.
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k401Unauthorized);
            response->setBody("Unauthorized: No token provided");
            fcb(response);
            return;
        }
    }
};
