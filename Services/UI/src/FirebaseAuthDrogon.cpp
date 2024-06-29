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
#include <QDebug>
#include <nlohmann/json.hpp>

#include <drogon/HttpSimpleController.h>
#include <drogon/drogon.h>
#include <httplib.h>

#include <chrono>
#include <future>
#include <thread>

using json = nlohmann::json;

using namespace DCS::UI;

FirebaseAuth::FirebaseAuth(const std::string_view& firebaseApiKey,
                           std::shared_ptr<DCS::Utils::Logger>& logger,
                           std::shared_ptr<RedisHandler>& redisHandler,
                           QObject* parent)
    : QObject(parent)
    , m_firebaseApiKey{firebaseApiKey}
    , m_logger{logger}
{
    m_httpClient = drogon::HttpClient::newHttpClient("http://identitytoolkit.googleapis.com");
}

std::string FirebaseAuth::getFullUrl(const std::string& path) const
{
    return "https://identitytoolkit.googleapis.com" + path;
}

void FirebaseAuth::registerUser(const QString& email, const QString& password)
{
    std::string path = "/v1/accounts:signUp?key=" + m_firebaseApiKey;
    nlohmann::json payload = {{"email", email.toStdString()}, {"password", password.toStdString()}, {"returnSecureToken", true}};

    auto req = drogon::HttpRequest::newHttpJsonRequest(payload.dump());
    req->setPath(path);
    DCS_LOG_INFO(m_logger, payload.dump());

    m_httpClient->sendRequest(req, [this](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        DCS_LOG_INFO(m_logger, "HTTP Request Result: " << result);
        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::HttpStatusCode::k200OK)
        {
            auto jsonResponse = json::parse(response->getBody());
            if (jsonResponse.contains("idToken"))
            {
                DCS_LOG_INFO(m_logger, "Registration successful");
                emit registrationSuccess();
            }
            else if (jsonResponse.contains("error"))
            {
                QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
                DCS_LOG_ERROR(m_logger, error.toStdString());
                emit registrationFailure(error);
            }
        }
        else
        {
            QString error = "Network error or invalid response";
            DCS_LOG_ERROR(m_logger, error.toStdString());
            emit registrationFailure(error);
        }
    });
}

void FirebaseAuth::loginUser(const QString& email, const QString& password)
{
    emit loginStarted();

    std::jthread([=]() {
        std::string path = "/v1/accounts:signInWithPassword?key=" + m_firebaseApiKey;
        nlohmann::json payload2 = {{"email", email.toStdString()}, {"password", password.toStdString()}, {"returnSecureToken", true}};

        nlohmann::json payload = {{"email", "user@example.com"}, {"password", "your_password"}, {"returnSecureToken", true}};

        auto req = drogon::HttpRequest::newHttpJsonRequest(payload.dump());
        // req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        // req->setMethod(drogon::HttpMethod::Post);
        req->setPath(path);

        std::promise<std::pair<drogon::ReqResult, drogon::HttpResponsePtr>> promise;
        auto future = promise.get_future();

        m_httpClient->sendRequest(req, [&promise](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
            promise.set_value({result, response});
        });

        if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
        {
            QString error = "Request timed out";
            DCS_LOG_ERROR(m_logger, error.toStdString() << ": " << getFullUrl(path));
            DCS_LOG_INFO(m_logger, "Payload: " << payload.dump());
            emit loginFailure(error);
            return;
        }

        auto [result, response] = future.get();
        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::HttpStatusCode::k200OK)
        {
            auto jsonResponse = json::parse(response->getBody());
            if (jsonResponse.contains("idToken"))
            {
                DCS_LOG_INFO(m_logger, "Registration successful");
                emit loginSuccess();
            }
            else if (jsonResponse.contains("error"))
            {
                QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
                DCS_LOG_ERROR(m_logger, error.toStdString());
                emit loginFailure(error);
            }
        }
        else
        {
            DCS_LOG_INFO(m_logger, "response->getStatusCode(): " << response->getStatusCode());
            QString error = "Network error or invalid response";
            DCS_LOG_ERROR(m_logger, error.toStdString());
            DCS_LOG_ERROR(m_logger, error.toStdString() << ": " << getFullUrl(path));
            DCS_LOG_INFO(m_logger, "Payload: " << payload.dump());
            emit loginFailure(error);
        }
    }).detach(); // Detach the thread to avoid blocking the main thread
}

void FirebaseAuth::logoutUser()
{
    // Firebase doesn't have a specific logout endpoint
    // Typically, you would invalidate the session on your server
    // and instruct the client to remove the stored token

    // Clear the stored token (implement this based on your storage method)
    // clearStoredToken();

    emit logoutSuccess();
}

void FirebaseAuth::sendPasswordResetEmail(const QString& email)
{
    std::string path = "/v1/accounts:sendOobCode?key=" + m_firebaseApiKey;
    nlohmann::json payload = {{"requestType", "PASSWORD_RESET"}, {"email", email.toStdString()}};

    auto req = drogon::HttpRequest::newHttpJsonRequest(payload.dump());
    req->setPath(path);

    m_httpClient->sendRequest(req, [this](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::HttpStatusCode::k200OK)
        {
            auto jsonResponse = json::parse(response->getBody());
            if (jsonResponse.contains("email"))
            {
                DCS_LOG_INFO(m_logger, "Registration successful");
                emit passwordResetSuccess();
            }
            else if (jsonResponse.contains("error"))
            {
                QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
                DCS_LOG_ERROR(m_logger, error.toStdString());
                emit passwordResetFailure(error);
            }
        }
        else
        {
            QString error = "Network error or invalid response";
            DCS_LOG_ERROR(m_logger, error.toStdString());
            emit passwordResetFailure(error);
        }
    });
}
