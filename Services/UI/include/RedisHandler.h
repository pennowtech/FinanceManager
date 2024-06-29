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

#include <chrono>
#include <memory>
#include <string>
#include <sw/redis++/redis++.h>

namespace DCS
{
    namespace UI
    {
        class RedisHandler
        {
        public:
            RedisHandler(const std::string& host, int port);

            void saveToken(const std::string& key, const std::string& token, std::chrono::seconds ttl);
            std::optional<std::string> getToken(const std::string& key);
            void deleteToken(const std::string& key);
            std::optional<int> getTTL(const std::string& key);

        private:
            std::shared_ptr<sw::redis::Redis> redis;
        };

    } // namespace UI
} // namespace DCS
