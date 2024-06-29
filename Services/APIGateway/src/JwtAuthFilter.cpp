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
            auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);

            // Extract the header to get the kid
            auto kid = decoded.get_header_claim("kid").as_string();

            // Find the corresponding public key
            if (publicKeysMap.find(kid) == publicKeysMap.end())
            {
                std::cerr << "kid (" << kid << ") not found.\n";
                return true;
            }
            auto pub_key = publicKeysMap.at(kid);

            auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
                                .allow_algorithm(jwt::algorithm::rs256{pub_key})
                                .with_issuer("https://securetoken.google.com/" + projectId); // Replace with your Firebase project ID

            verifier.verify(decoded);
            std::cerr << "Authorization token verified.\n";

            // Token is valid; continue the request
            fccb();
        }
        catch (const jwt::error::invalid_json_exception& e)
        {
            std::cerr << "Token parsing error: " << e.what() << "\n";
        }
        catch (const jwt::error::token_verification_exception& e)
        {
            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k401Unauthorized);
            response->setBody("Unauthorized: Invalid token");
            std::cerr << "Unauthorized: Invalid token. " << e.what() << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "Handle other standard exceptions. " << e.what() << "\n";
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
