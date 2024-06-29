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

#include <jwt-cpp/jwt.h>

#include <string>

bool validateJwt(const std::string& token, const std::string& publicKey)
{
    try
    {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::rs256(publicKey)) // This logic is explained above on how to get public key
                            .with_issuer("<https://securetoken.google.com/your-firebase-project-id>");

        verifier.verify(decoded);
        return true;
    }
    catch (...)
    {
        return false;
    }
}
