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

#include <Utils / LoggerQuill.h>

#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/RotatingFileSink.h>

#include <sstream>

namespace DCS::Utils
{
    LoggerQuill::LoggerQuill(std::string loggerName, LogLevel logLevel, bool console)
        : m_logger(nullptr)
    {
        std::filesystem::path log_files_path = std::filesystem::path(std::getenv("HOME")) / "logs";
        std::filesystem::create_directories(log_files_path);

        quill::Backend::start();

        auto rotating_file_sink =
            quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(log_files_path.string() + "/" + loggerName + ".log", []() {
                // See RotatingFileSinkConfig for more options
                quill::RotatingFileSinkConfig cfg;
                cfg.set_open_mode('w');
                cfg.set_filename_append_option(quill::FilenameAppendOption::StartDateTime);
                cfg.set_rotation_time_daily("18:30");
                cfg.set_rotation_max_file_size(1024);
                return cfg;
            }());

        if (console)
        {
            auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(loggerName + "console_sink");
            m_logger = quill::Frontend::create_or_get_logger(
                loggerName, {std::move(rotating_file_sink), std::move(console_sink)}, "%(time) | %(log_level:<6) | %(logger:<15) %(message)");
        }
        else
        {
            m_logger = quill::Frontend::create_or_get_logger(loggerName, std::move(rotating_file_sink));
        }

        m_logger->set_log_level(toQuillLogLevel(logLevel));
    }

    void LoggerQuill::log(LogLevel level, std::string_view message, const char* file, int line, const char* function)
    {
        std::string logMessage{" | "};

        std::filesystem::path filePath(file ? file : "");
        std::string fileName = filePath.filename().string();

        if (!fileName.empty())
        {
            logMessage += fileName;
        }

        if (line != 0)
        {
            logMessage += ":" + std::to_string(line);
        }

        if (function != nullptr)
        {
            logMessage += ", " + std::string(function) + "()";
        }

        logMessage += (" | " + std::string(message));
        printLogs(level, logMessage);
    }

    void LoggerQuill::printLogs(LogLevel level, std::string& logMessage)
    {
        switch (level)
        {
            case LogLevel::Trace:
                LOG_TRACE_L1(m_logger, "{}", logMessage);
                break;
            case LogLevel::Debug:
                LOG_DEBUG(m_logger, "{}", logMessage);
                break;
            case LogLevel::Info:
                LOG_INFO(m_logger, "{}", logMessage);
                break;
            case LogLevel::Warn:
                LOG_WARNING(m_logger, "{}", logMessage);
                break;
            case LogLevel::Error:
                LOG_ERROR(m_logger, "{}", logMessage);
                break;
            case LogLevel::Fatal:
                LOG_CRITICAL(m_logger, "{}", logMessage);
                break;
            default:
                LOG_WARNING(m_logger, "Invalid log level supplied for: {}", logMessage);
                break;
        }
    }

    quill::LogLevel LoggerQuill::toQuillLogLevel(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Trace:
                return quill::LogLevel::TraceL1;
            case LogLevel::Debug:
                return quill::LogLevel::Debug;
            case LogLevel::Info:
                return quill::LogLevel::Info;
            case LogLevel::Warn:
                return quill::LogLevel::Warning;
            case LogLevel::Error:
                return quill::LogLevel::Error;
            case LogLevel::Fatal:
                return quill::LogLevel::Critical;
            default:
                return quill::LogLevel::Info;
                break;
        }
    }
} // namespace DCS::Utils
