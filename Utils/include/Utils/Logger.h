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

#include <sstream>

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
};

#define LOG(level, logger, message)                                                                                                                  \
    do                                                                                                                                               \
    {                                                                                                                                                \
        std::ostringstream oss;                                                                                                                      \
        oss << message;                                                                                                                              \
        logger->log(level, oss.str(), __FILE__, __LINE__, __func__);                                                                                 \
    } while (0)

#define DCS_LOG_TRACE(logger, message) LOG(LogLevel::Info, logger, message)
#define DCS_LOG_INFO(logger, message) LOG(LogLevel::Info, logger, message)
#define DCS_LOG_DEBUG(logger, message) LOG(LogLevel::Debug, logger, message)
#define DCS_LOG_WARN(logger, message) LOG(LogLevel::Error, logger, message)
#define DCS_LOG_ERROR(logger, message) LOG(LogLevel::Error, logger, message)

#define ENUM_TO_STRING(EnumType, ...)                                                                                                                \
    inline std::string EnumType##ToString(EnumType value)                                                                                            \
    {                                                                                                                                                \
        switch (value)                                                                                                                               \
        {                                                                                                                                            \
            __VA_ARGS__                                                                                                                              \
            default:                                                                                                                                 \
                return "INVALID";                                                                                                                    \
        }                                                                                                                                            \
    }

// Macro to create case statements for each enum value
#define ENUM_CASE(EnumValue)                                                                                                                         \
    case EnumValue:                                                                                                                                  \
        return #EnumValue;

namespace DCS::Utils
{
    class Logger
    {
    public:
        inline virtual void log(LogLevel level, std::string_view message, const char* file, int line, const char* function) = 0;
        virtual ~Logger() = default;

    private:
        virtual void printLogs(LogLevel level, std::string& logMessage) = 0;
    };

} // namespace DCS::Utils
