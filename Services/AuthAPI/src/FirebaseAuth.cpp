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
#include <FirebaseAuth.h>

FirebaseAuth::FirebaseAuth()
{
    firebaseApiKey = "YOUR_FIREBASE_API_KEY";
}

void FirebaseAuth::registerUser(const std::string& email, const std::string& password)
{
    std::string path = "/v1/accounts:signUp?key=" + firebaseApiKey;
    json payload = {{"email", email}, {"password", password}, {"returnSecureToken", true}};

    std::string response = performPostRequest(path, payload);
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("idToken"))
    {
        emit loginSuccess();
    }
    else
    {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit loginFailure(error);
    }
}

void FirebaseAuth::loginUser(const std::string& email, const std::string& password, const std::function<void(const std::string&)>& callback)
{
    std::string path = "/v1/accounts:signInWithPassword?key=" + firebaseApiKey;
    json payload = {{"email", email.toStdString()}, {"password", password.toStdString()}, {"returnSecureToken", true}};

    std::string response = performPostRequest(path, payload);
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("idToken"))
    {
        emit loginSuccess();
    }
    else
    {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit loginFailure(error);
    }
}

void FirebaseAuth::sendPasswordResetEmail(const std::string& email)
{
    std::string path = "/v1/accounts:sendOobCode?key=" + firebaseApiKey;
    json payload = {{"requestType", "PASSWORD_RESET"}, {"email", email}};

    std::string response = performPostRequest(path, payload);
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("email"))
    {
        emit passwordResetSuccess();
    }
    else
    {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit passwordResetFailure(error);
    }
}

json FirebaseAuth::performPostRequest(const std::string& path, const std::string& payload)
{
    auto client = drogon::HttpClient::newHttpClient("https://identitytoolkit.googleapis.com");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::HttpMethod::Post);
    req->setPath(path);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req->setBody(payload.dump());

    client->sendRequest(req, [this](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        if (result == ReqResult::Ok && response->getStatusCode() == k200OK)
        {
            auto jsonResponse = json::parse(response->getBody());
            return jsonResponse;
        }
        else
        {
            std::cerr << "Failed to login: " << response->getReasonPhrase() << std::endl;
        }
    });
}
