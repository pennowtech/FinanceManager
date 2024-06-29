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
#include <Utils/Logger.h>
#include <Utils/LoggerQuill.h>

#include <RedisHandler.h>

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>

#include <QObject>
#include <string>

namespace DCS
{
    namespace UI
    {
        class FirebaseAuth : public QObject
        {
            Q_OBJECT
        public:
            FirebaseAuth(const std::string_view& firebaseApiKey,
                         std::shared_ptr<DCS::Utils::Logger>& logger,
                         std::shared_ptr<RedisHandler>& redisHandler,
                         QObject* parent = nullptr);
            virtual ~FirebaseAuth() = default;

            Q_INVOKABLE virtual void registerUser(const QString& email, const QString& password) = 0;
            Q_INVOKABLE virtual void loginUser(const QString& email, const QString& password) = 0;
            Q_INVOKABLE virtual void sendPasswordResetEmail(const QString& email) = 0;
            Q_INVOKABLE virtual void logoutUser() = 0;
            Q_INVOKABLE virtual void renewToken() = 0;

            void initializeTokenRenewal();

        signals:
            void authOpStarted();
            void registrationSuccess();
            void registrationFailure(const QString& error);
            void loginSuccess();
            void loginFailure(const QString& error);
            void logoutSuccess();
            void passwordResetSuccess();
            void passwordResetFailure(const QString& error);

            void tokenRenewed();
            void tokenRenewalFailed(const QString& error);
            void tokenRenewalRequested();
            void tokenRenewalConfirmed();

        protected:
            std::string getFullUrl(const std::string& path) const;
            void scheduleTokenRenewal(int expiresIn);
            void promptTokenRenewal();

        protected:
            std::string m_firebaseApiKey;
            std::shared_ptr<DCS::Utils::Logger> m_logger;
            std::shared_ptr<RedisHandler> m_redisHandler;
        };
    } // namespace UI
} // namespace DCS
