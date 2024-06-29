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
#include <Utils/EnvReader.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

EnvReader::EnvReader(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    file >> m_config;
}

template<typename T>
T EnvReader::getValueOrDefault(const std::string& serviceName, const std::string& key, const T& defaultValue)
{
    // DCS_LOG_INFO(logger, "serviceName: " << serviceName << ", key: " << key);

    // Attempt to fetch the serviceName-specific setting
    if (m_config.contains(serviceName) && m_config[serviceName].contains(key))
    {
        return m_config[serviceName][key].get<T>();
    }
    return m_config.value(key, defaultValue);
}

// Template instantiations for common types
template std::string EnvReader::getValueOrDefault<std::string>(const std::string&, const std::string&, const std::string&);
template int EnvReader::getValueOrDefault<int>(const std::string&, const std::string&, const int&);
template bool EnvReader::getValueOrDefault<bool>(const std::string&, const std::string&, const bool&);
