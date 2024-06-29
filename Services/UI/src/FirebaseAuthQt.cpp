#include "FirebaseAuthQt.h"
#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <chrono>
#include <future>
#include <thread>

namespace DCS::UI
{
    FirebaseAuthQt::FirebaseAuthQt(const std::string_view& firebaseApiKey,
                                   std::shared_ptr<DCS::Utils::Logger>& logger,
                                   std::shared_ptr<RedisHandler>& redisHandler,
                                   QObject* parent)
        : FirebaseAuth(firebaseApiKey, logger, redisHandler, parent)
        , m_networkManager{new QNetworkAccessManager(this)}
    {
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &FirebaseAuthQt::onLoginFinished);
        connect(this, &FirebaseAuth::tokenRenewalConfirmed, this, &FirebaseAuthQt::renewToken);
        DCS_LOG_INFO(m_logger, "Initializing FirebaseAuthQt");
    }

    void FirebaseAuthQt::registerUser(const QString& email, const QString& password)
    {
        emit authOpStarted();

        QJsonObject payload;
        payload["email"] = email;
        payload["password"] = password;
        payload["returnSecureToken"] = true;

        QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=%1").arg(m_firebaseApiKey.c_str()));

        auto onSuccess = [this](const nlohmann::json& jsonResponse) {
            if (jsonResponse.contains("idToken") && jsonResponse.contains("refreshToken") && jsonResponse.contains("expiresIn"))
            {
                std::string idToken = jsonResponse["idToken"];
                std::string refreshToken = jsonResponse["refreshToken"];
                int expiresIn = jsonResponse["expiresIn"];

                DCS_LOG_INFO(m_logger, "Registration successful");

                // Save the tokens in Redis with expiry times
                m_redisHandler->saveToken("session_token", idToken, std::chrono::seconds(expiresIn));
                m_redisHandler->saveToken("refresh_token", refreshToken, std::chrono::days(30));

                scheduleTokenRenewal(expiresIn);

                emit registrationSuccess();
            }
        };

        auto onFailure = [this](const QString& error) {
            DCS_LOG_WARN(m_logger, error.toStdString());
            emit registrationFailure(error);
        };

        sendRequest(url, payload, onSuccess, onFailure, "Request timed out while registering user.");
    }

    void FirebaseAuthQt::loginUser(const QString& email, const QString& password)
    {
        emit authOpStarted();

        QJsonObject payload;
        payload["email"] = email;
        payload["password"] = password;
        payload["returnSecureToken"] = true;

        QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=%1").arg(m_firebaseApiKey.c_str()));

        auto onSuccess = [this](const nlohmann::json& jsonResponse) {
            if (jsonResponse.contains("idToken") && jsonResponse.contains("refreshToken") && jsonResponse.contains("expiresIn"))
            {
                std::string idToken = jsonResponse["idToken"];
                std::string refreshToken = jsonResponse["refreshToken"];
                int expiresIn = jsonResponse["expiresIn"];

                DCS_LOG_INFO(m_logger, "Login successful");

                // Save the tokens in Redis with expiry times
                m_redisHandler->saveToken("session_token", idToken, std::chrono::seconds(expiresIn));
                m_redisHandler->saveToken("refresh_token", refreshToken, std::chrono::days(30));

                scheduleTokenRenewal(expiresIn);

                emit loginSuccess();
            }
        };

        auto onFailure = [this](const QString& error) {
            DCS_LOG_WARN(m_logger, error.toStdString());
            emit loginFailure(error);
        };

        sendRequest(url, payload, onSuccess, onFailure, "Request timed out while logging in.");
    }

    void FirebaseAuthQt::onLoginFinished(QNetworkReply* reply)
    {
        auto onSuccess = [this](const nlohmann::json& jsonResponse) {
            if (jsonResponse.contains("idToken") && jsonResponse.contains("refreshToken") && jsonResponse.contains("expiresIn"))
            {
                std::string idToken = jsonResponse["idToken"];
                std::string refreshToken = jsonResponse["refreshToken"];
                int expiresIn = jsonResponse["expiresIn"];

                DCS_LOG_INFO(m_logger, "Login successful");

                // Save the tokens in Redis with expiry times
                m_redisHandler->saveToken("session_token", idToken, std::chrono::seconds(expiresIn));
                m_redisHandler->saveToken("refresh_token", refreshToken, std::chrono::days(30));

                scheduleTokenRenewal(expiresIn);

                emit loginSuccess();
            }
        };

        auto onFailure = [this](const QString& error) {
            DCS_LOG_WARN(m_logger, error.toStdString());
            emit loginFailure(error);
        };
        DCS_LOG_INFO(m_logger, "Going to handle network reply");

        handleNetworkReply(reply, onSuccess, onFailure);
    }

    void FirebaseAuthQt::sendPasswordResetEmail(const QString& email)
    {
        emit authOpStarted();

        QJsonObject payload;
        payload["requestType"] = "PASSWORD_RESET";
        payload["email"] = email;

        QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode?key=%1").arg(m_firebaseApiKey.c_str()));

        auto onSuccess = [this](const nlohmann::json& jsonResponse) {
            DCS_LOG_INFO(m_logger, "Password reset email sent successfully");
            emit passwordResetSuccess();
        };

        auto onFailure = [this](const QString& error) {
            DCS_LOG_WARN(m_logger, error.toStdString());
            emit passwordResetFailure(error);
        };

        sendRequest(url, payload, onSuccess, onFailure, "Request timed out while sending password reset email.");
    }

    void FirebaseAuthQt::renewToken()
    {
        DCS_LOG_INFO(m_logger, "Renewing token...");
        auto refreshTokenOpt = m_redisHandler->getToken("refresh_token");
        if (!refreshTokenOpt.has_value())
        {
            QString error = "No refresh token available";
            DCS_LOG_ERROR(m_logger, error.toStdString());
            emit tokenRenewalFailed(error);
            return;
        }

        QString refreshToken = QString::fromStdString(refreshTokenOpt.value());

        QUrl url(QString("https://securetoken.googleapis.com/v1/token?key=%1").arg(m_firebaseApiKey.c_str()));

        QJsonObject payload;
        payload["grant_type"] = "refresh_token";
        payload["refresh_token"] = refreshToken;

        auto onSuccess = [this](const nlohmann::json& jsonResponse) {
            if (jsonResponse.contains("id_token") && jsonResponse.contains("refresh_token"))
            {
                std::string idToken = jsonResponse["id_token"];
                std::string newRefreshToken = jsonResponse["refresh_token"];
                int expiresIn = jsonResponse["expires_in"];

                DCS_LOG_INFO(m_logger, "Token renewed successfully");

                // Save the new tokens in Redis with expiry times
                m_redisHandler->saveToken("session_token", idToken, std::chrono::seconds(expiresIn));
                m_redisHandler->saveToken("refresh_token", newRefreshToken, std::chrono::days(30));
                DCS_LOG_INFO(m_logger, "Token renewed");

                emit tokenRenewed();
            }
        };

        auto onFailure = [this](const QString& error) {
            DCS_LOG_WARN(m_logger, error.toStdString());
            emit tokenRenewalFailed(error);
        };

        sendRequest(url, payload, onSuccess, onFailure, "Request timed out while renewing token.");
    }

    void FirebaseAuthQt::logoutUser()
    {
        emit authOpStarted();
        DCS_LOG_INFO(m_logger, "");

        std::jthread([=]() {
            // Invalidate the session token in Redis
            m_redisHandler->deleteToken("session_token");
            m_redisHandler->deleteToken("refresh_token");

            emit logoutSuccess();
        }).detach(); // Detach the thread to avoid blocking the main thread
    }

    void FirebaseAuthQt::sendRequest(const QUrl& url,
                                     const QJsonObject& payload,
                                     std::function<void(const nlohmann::json&)> onSuccess,
                                     std::function<void(const QString&)> onFailure,
                                     const QString& timeoutErrorMessage,
                                     int timeoutSeconds)
    {
        DCS_LOG_INFO(m_logger, "");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonDocument doc(payload);
        QByteArray data = doc.toJson();

        auto reply = m_networkManager->post(request, data);

        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        timer.start(timeoutSeconds * 1000);
        loop.exec();

        if (timer.isActive())
        {
            if (reply->error() != QNetworkReply::NoError)
            {
                QString error = "Network error: " + reply->errorString();
                DCS_LOG_ERROR(m_logger, error.toStdString());
                onFailure(error);
            }
            else
            {
                handleNetworkReply(reply, onSuccess, onFailure);
            }
        }
        else
        {
            // Timeout
            QString error = timeoutErrorMessage;
            DCS_LOG_ERROR(m_logger, error.toStdString());
            reply->abort();
            onFailure(error);
        }

        reply->deleteLater();
    }

    void FirebaseAuthQt::handleNetworkReply(QNetworkReply* reply,
                                            std::function<void(const nlohmann::json&)> onSuccess,
                                            std::function<void(const QString&)> onFailure)
    {
        DCS_LOG_TRACE(m_logger, "");
        QByteArray response_data = reply->readAll();
        auto jsonResponse = nlohmann::json::parse(response_data.toStdString());

        if (jsonResponse.contains("error"))
        {
            QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
            DCS_LOG_ERROR(m_logger, error.toStdString());
            onFailure(error);
        }
        else
        {
            onSuccess(jsonResponse);
        }
    }
} // namespace DCS::UI
