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

#include <QObject>
#include <QString>
#include <cassandra.h>
#include <memory>

class CassandraDB : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cassandraStatus READ cassandraStatus NOTIFY statusChanged)
public:
    explicit CassandraDB(QObject* parent = nullptr);
    QString cassandraStatus() const;

    void setSettings(const QString& cassandraHost, bool enableCaching);
    Q_INVOKABLE void testConnections(const QString& cassandraHost);
    Q_INVOKABLE void applySettings(const QString& cassandraHost, bool enableCaching);

    void saveTransaction(const QString& amount, const QString& category);
    void loadTransactions();
    void setBudget(const QString& category, int amount);

signals:
    void statusChanged();
    void transactionsModelChanged();

private:
    QString m_cassandraStatus;
    std::shared_ptr<CassCluster> m_cluster;
    std::shared_ptr<CassSession> m_session;
};
