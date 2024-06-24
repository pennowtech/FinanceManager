#include "DBManager.h"

#include <sstream>

DBManager::DBManager(QObject* parent)
    : QObject(parent), m_cluster(cass_cluster_new()), m_session(cass_session_new()), m_enableCaching(false) {
    m_cassandraStatus = "Disconnected";
    m_redisStatus = "Disconnected";
}

QString DBManager::cassandraStatus() const { return m_cassandraStatus; }

QString DBManager::redisStatus() const { return m_redisStatus; }

void DBManager::setSettings(const QString& cassandraHost, const QString& redisHost, bool enableCaching) {

}

void DBManager::testConnections(const QString& cassandraHost, const QString& redisHost) {
    // Test Cassandra connection
    CassCluster* test_cluster = cass_cluster_new();
    cass_cluster_set_contact_points(test_cluster, cassandraHost.toStdString().c_str());
    CassSession* test_session = cass_session_new();
    CassFuture* connect_future = cass_session_connect(test_session, test_cluster);
    if (cass_future_error_code(connect_future) == CASS_OK) {
        m_testCassandraStatus = "Connected";
    } else {
        m_testCassandraStatus = "Failed to connect";
    }
    cass_future_free(connect_future);
    cass_session_free(test_session);
    cass_cluster_free(test_cluster);

    // Test Redis connection
    cpp_redis::client test_redisClient;
    test_redisClient.connect(redisHost.toStdString(), 6379);
    if (test_redisClient.is_connected()) {
        m_testRedisStatus = "Connected";
    } else {
        m_testRedisStatus = "Failed to connect";
    }

    emit statusChanged();
}

void DBManager::applySettings(const QString& cassandraHost, const QString& redisHost, bool enableCaching) {
    setSettings(cassandraHost, redisHost, enableCaching);
}

void DBManager::saveTransaction(const QString& amount, const QString& category) {
    // Save to Cassandra
    CassStatement* statement =
        cass_statement_new("INSERT INTO finance.transactions (id, amount, category) VALUES (uuid(), ?, ?)", 2);
    cass_statement_bind_string(statement, 0, amount.toStdString().c_str());
    cass_statement_bind_string(statement, 1, category.toStdString().c_str());

    CassFuture* result_future = cass_session_execute(m_session.get(), statement);
    if (cass_future_error_code(result_future) != CASS_OK) {
        qWarning() << "Failed to insert transaction into Cassandra";
    }
    cass_future_free(result_future);
    cass_statement_free(statement);

    // Save to Redis (for recent transactions)
    if (m_enableCaching) {
        std::stringstream ss;
        ss << "transaction:" << amount.toStdString() << ":" << category.toStdString();
        m_redisClient.lpush("recent_transactions", {ss.str()});
        m_redisClient.sync_commit();
    }
}

std::vector<std::pair<QString, QString>> DBManager::loadTransactions() {
    std::vector<std::pair<QString, QString>> transactions;

    // Try loading from Redis cache first
    if (m_enableCaching) {
        auto reply = m_redisClient.lrange("recent_transactions", 0, -1);
        m_redisClient.sync_commit();
        if (reply.get().is_array()) {
            for (const auto& item : reply.get().as_array()) {
                std::string transaction = item.as_string();
                size_t pos = transaction.find(':');
                if (pos != std::string::npos) {
                    QString amount = QString::fromStdString(transaction.substr(12, pos - 12));
                    QString category = QString::fromStdString(transaction.substr(pos + 1));
                    transactions.emplace_back(amount, category);
                }
            }
            return transactions;
        }
    }

    // Load from Cassandra if not found in Redis
    CassStatement* statement = cass_statement_new("SELECT amount, category FROM finance.transactions", 0);
    CassFuture* result_future = cass_session_execute(m_session.get(), statement);

    const CassResult* result = cass_future_get_result(result_future);
    if (result) {
        CassIterator* rows = cass_iterator_from_result(result);
        while (cass_iterator_next(rows)) {
            const CassRow* row = cass_iterator_get_row(rows);
            const CassValue* amount_value = cass_row_get_column_by_name(row, "amount");
            const CassValue* category_value = cass_row_get_column_by_name(row, "category");

            const char* amount;
            const char* category;
            size_t amount_length;
            size_t category_length;

            cass_value_get_string(amount_value, &amount, &amount_length);
            cass_value_get_string(category_value, &category, &category_length);

            transactions.emplace_back(QString::fromUtf8(amount, amount_length),
                                      QString::fromUtf8(category, category_length));
        }
        cass_result_free(result);
        cass_iterator_free(rows);
    }
    cass_future_free(result_future);
    cass_statement_free(statement);

    return transactions;
}

void DBManager::setBudget(const QString& category, int amount) {
    // Save budget to Cassandra
    CassStatement* statement = cass_statement_new("INSERT INTO finance.budgets (category, amount) VALUES (?, ?)", 2);
    cass_statement_bind_string(statement, 0, category.toStdString().c_str());
    cass_statement_bind_int32(statement, 1, amount);

    CassFuture* result_future = cass_session_execute(m_session.get(), statement);
    if (cass_future_error_code(result_future) != CASS_OK) {
        qWarning() << "Failed to set budget in Cassandra";
    }
    cass_future_free(result_future);
    cass_statement_free(statement);
}