#include <RedisHandler.h>

namespace DCS::UI
{
    RedisHandler::RedisHandler(const std::string& host, int port)
    {
        sw::redis::ConnectionOptions connection_options;
        connection_options.host = host;
        connection_options.port = port;
        redis = std::make_shared<sw::redis::Redis>(connection_options);
    }

    void RedisHandler::saveToken(const std::string& key, const std::string& token, std::chrono::seconds ttl)
    {
        redis->set(key, token);
        redis->expire(key, ttl);
    }

    std::optional<std::string> RedisHandler::getToken(const std::string& key)
    {
        auto val = redis->get(key);
        if (val)
        {
            return *val;
        }
        return std::nullopt;
    }

    void RedisHandler::deleteToken(const std::string& key)
    {
        redis->del(key);
    }

    std::optional<int> RedisHandler::getTTL(const std::string& key)
    {
        auto ttl = redis->ttl(key);
        if (ttl != -2)
        {
            return ttl;
        }
        return std::nullopt;
    }
} // namespace DCS::UI
