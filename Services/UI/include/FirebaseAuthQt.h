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
#include <FirebaseAuth.h>

#include <Utils/Logger.h>
#include <Utils/LoggerQuill.h>

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QObject>
#include <string>

namespace DCS
{
    namespace UI
    {
        class FirebaseAuthQt : public FirebaseAuth
        {
            Q_OBJECT
        public:
            FirebaseAuthQt(const std::string_view& firebaseApiKey,
                           std::shared_ptr<DCS::Utils::Logger>& logger,
                           std::shared_ptr<RedisHandler>& redisHandler,
                           QObject* parent = nullptr);

            Q_INVOKABLE void registerUser(const QString& email, const QString& password) override;
            Q_INVOKABLE void loginUser(const QString& email, const QString& password) override;
            Q_INVOKABLE void sendPasswordResetEmail(const QString& email) override;
            Q_INVOKABLE void logoutUser() override;
            Q_INVOKABLE void renewToken() override;

        private slots:
            void onLoginFinished(QNetworkReply* reply);

        private:
            void sendRequest(const QUrl& url,
                             const QJsonObject& payload,
                             std::function<void(const nlohmann::json&)> onSuccess,
                             std::function<void(const QString&)> onFailure,
                             const QString& timeoutErrorMessage,
                             int timeoutSeconds = 5);
            void handleNetworkReply(QNetworkReply* reply,
                                    std::function<void(const nlohmann::json&)> onSuccess,
                                    std::function<void(const QString&)> onFailure);

        private:
            QNetworkAccessManager* m_networkManager;
        };
    } // namespace UI
} // namespace DCS
