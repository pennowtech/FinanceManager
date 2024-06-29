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
#include "CassandraDB.h"

#include <cassandra.h>

#include <sstream>

CassandraDB::CassandraDB(QObject* parent)
    : QObject(parent)
    , m_cluster(cass_cluster_new())
    , m_session(cass_session_new())
{
    m_cassandraStatus = "Disconnected";
}

QString CassandraDB::cassandraStatus() const
{
    return m_cassandraStatus;
}

void CassandraDB::setSettings(const QString& cassandraHostRedisDB, bool enableCaching)
{
    m_enableCaching = enableCaching;

    // Initialize Cassandra connection
    cass_cluster_set_contact_points(m_cluster.get(), cassandraHost.toStdString().c_str());
    CassFuture* connect_future = cass_session_connect(m_session.get(), m_cluster.get());
    if (cass_future_error_code(connect_future) == CASS_OK)
    {
        m_cassandraStatus = "Connected";
    }
    else
    {
        m_cassandraStatus = "Failed to connect";
    }
    cass_future_free(connect_future);

    emit statusChanged();
}

void CassandraDB::testConnections(const QString& cassandraHostRedisDB)
{
    // Test Cassandra connection
    CassCluster* test_cluster = cass_cluster_new();
    cass_cluster_set_contact_points(test_cluster, cassandraHost.toStdString().c_str());
    CassSession* test_session = cass_session_new();
    CassFuture* connect_future = cass_session_connect(test_session, test_cluster);
    if (cass_future_error_code(connect_future) == CASS_OK)
    {
        m_testCassandraStatus = "Connected";
    }
    else
    {
        m_testCassandraStatus = "Failed to connect";
    }
    cass_future_free(connect_future);
    cass_session_free(test_session);
    cass_cluster_free(test_cluster);

    emit statusChanged();
}

void CassandraDB::applySettings(const QString& cassandraHostRedisDB)
{
    setSettings(cassandraHost);
}

void CassandraDB::saveTransaction(const QString& amount, const QString& category)
{
    // Save to Cassandra
    CassStatement* statement = cass_statement_new("INSERT INTO finance.transactions (id, amount, category) VALUES (uuid(), ?, ?)", 2);
    cass_statement_bind_string(statement, 0, amount.toStdString().c_str());
    cass_statement_bind_string(statement, 1, category.toStdString().c_str());

    CassFuture* result_future = cass_session_execute(m_session.get(), statement);
    if (cass_future_error_code(result_future) != CASS_OK)
    {
        qWarning() << "Failed to insert transaction into Cassandra";
    }
    cass_future_free(result_future);
    cass_statement_free(statement);
}

std::vector<std::pair<QString, QString>> CassandraDB::loadTransactions()
{
    std::vector<std::pair<QString, QString>> transactions;

    // Load from Cassandra if not found in Redis
    CassStatement* statement = cass_statement_new("SELECT amount, category FROM finance.transactions", 0);
    CassFuture* result_future = cass_session_execute(m_session.get(), statement);

    const CassResult* result = cass_future_get_result(result_future);
    if (result)
    {
        CassIterator* rows = cass_iterator_from_result(result);
        while (cass_iterator_next(rows))
        {
            const CassRow* row = cass_iterator_get_row(rows);
            const CassValue* amount_value = cass_row_get_column_by_name(row, "amount");
            const CassValue* category_value = cass_row_get_column_by_name(row, "category");

            const char* amount;
            const char* category;
            size_t amount_length;
            size_t category_length;

            cass_value_get_string(amount_value, &amount, &amount_length);
            cass_value_get_string(category_value, &category, &category_length);

            transactions.emplace_back(QString::fromUtf8(amount, amount_length), QString::fromUtf8(category, category_length));
        }
        cass_result_free(result);
        cass_iterator_free(rows);
    }
    cass_future_free(result_future);
    cass_statement_free(statement);

    return transactions;
}

void CassandraDB::setBudget(const QString& category, int amount)
{
    // Save budget to Cassandra
    CassStatement* statement = cass_statement_new("INSERT INTO finance.budgets (category, amount) VALUES (?, ?)", 2);
    cass_statement_bind_string(statement, 0, category.toStdString().c_str());
    cass_statement_bind_int32(statement, 1, amount);

    CassFuture* result_future = cass_session_execute(m_session.get(), statement);
    if (cass_future_error_code(result_future) != CASS_OK)
    {
        qWarning() << "Failed to set budget in Cassandra";
    }
    cass_future_free(result_future);
    cass_statement_free(statement);
}
