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

#include "quill/Backend.h"

#include <sstream>

namespace DCS::Utils
{
    class LoggerQuill : public Logger
    {
    public:
        LoggerQuill(std::string loggerName, LogLevel logLevel, bool console = false);

        inline void log(LogLevel level, std::string_view message, const char* file, int line, const char* function) override;

        LoggerQuill() = default;
        ~LoggerQuill() = default;

        LoggerQuill(const LoggerQuill&) = delete;
        LoggerQuill& operator=(const LoggerQuill&) = delete;

        LoggerQuill(LoggerQuill&&) = delete;
        LoggerQuill& operator=(LoggerQuill&&) = delete;

    private:
        void printLogs(LogLevel level, std::string& logMessage) override;
        quill::LogLevel toQuillLogLevel(LogLevel level);

    private:
        quill::Logger* m_logger;
    };

} // namespace DCS::Utils
