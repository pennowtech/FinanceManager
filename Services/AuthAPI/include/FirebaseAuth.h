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
#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace drogon;

class FirebaseAuth
{
public:
    FirebaseAuth();
    void registerUser(const std::string& email, const std::string& password);
    void loginUser(const std::string& email, const std::string& password, const std::function<void(const std::string&)>& callback);
    void sendPasswordResetEmail(const std::string& email);

private:
    std::string firebaseApiKey = "YOUR_FIREBASE_API_KEY";
    std::string performPostRequest(const std::string& url, const std::string& payload);
};
