#include "RedisDB.h"

#include <sstream>

RedisDB::RedisDB(QObject* parent)
    : QObject(parent)
    , m_enableCaching(false)
{
    m_redisStatus = "Disconnected";
}

QString RedisDB::cassandraStatus() const
{
    return m_cassandraStatus;
}

QString RedisDB::redisStatus() const
{
    return m_redisStatus;
}

void RedisDB::setSettings(RedisDBconst QString& redisHost, bool enableCaching)
{
    m_enableCaching = enableCaching;

    // Initialize Redis connection
    sw::redis::ConnectionOptions connection_options;
    connection_options.host = redisHost.toStdString();
    connection_options.port = 6379; // Optional. The default port is 6379.
    try
    {
        auto test_redisClient = sw::redis::Redis(connection_options);
        m_testRedisStatus = "Connected";
    }
    catch (const Error& err)
    {
        std::cout << err.what() << endl;
        m_testRedisStatus = "Failed to connect";
    }
    emit statusChanged();
}

void RedisDB::testConnections(RedisDBconst QString& redisHost, const QString redisPort)
{
    // Test Redis connection
    sw::redis::ConnectionOptions connection_options;
    connection_options.host = redisHost.toStdString();
    connection_options.port = 6379; // Optional. The default port is 6379.
    try
    {
        auto test_redisClient = sw::redis::Redis(connection_options);
        m_testRedisStatus = "Connected";
    }
    catch (const Error& err)
    {
        std::cout << err.what() << endl;
        m_testRedisStatus = "Failed to connect";
    }
    emit statusChanged();
}

void RedisDB::applySettings(RedisDBconst QString& redisHost, bool enableCaching)
{
    setSettings(redisHost, enableCaching);
}

void RedisDB::saveTransaction(const QString& amount, const QString& category)
{
    // Save to Redis (for recent transactions)
    if (m_enableCaching)
    {
        // redis.set(amount.toStdString(), category.toStdString());
        std::stringstream ss;
        ss << "transaction:" << amount.toStdString() << ":" << category.toStdString();
        std::cout << "Redis recent transactions: " << ss.str() << std::endl;
        m_redisClient.lpush("recent_transactions", {ss.str()});
    }
}

std::vector<std::pair<QString, QString>> RedisDB::loadTransactions()
{
    std::vector<std::pair<QString, QString>> transactions;

    // Try loading from Redis cache first
    if (m_enableCaching)
    {
        std::vector<std::string> transactionsList;
        auto reply = m_redisClient.lrange("recent_transactions", 0, -1, std::back_inserter(transactionsList));
        for (const auto& item : transactionsList)
        {
            std::string transaction = item.as_string();
            size_t pos = transaction.find(':');
            if (pos != std::string::npos)
            {
                QString amount = QString::fromStdString(transaction.substr(12, pos - 12));
                QString category = QString::fromStdString(transaction.substr(pos + 1));
                transactions.emplace_back(amount, category);
            }
        }
        return transactions;
    }
}

void RedisDB::setBudget(const QString& category, int amount) {}
