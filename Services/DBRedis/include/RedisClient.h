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
#include <memory>
#include <sw/redis++/redis++.h>

class RedisDB : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString redisStatus READ redisStatus NOTIFY statusChanged)
public:
    explicit RedisDB(QObject* parent = nullptr);
    QString redisStatus() const;

    void setSettings(const QString& redisHost, bool enableCaching);
    Q_INVOKABLE void testConnections(const QString& redisHost);
    Q_INVOKABLE void applySettings(const QString& redisHost, bool enableCaching);

    void saveTransaction(const QString& amount, const QString& category);
    void loadTransactions();
    void setBudget(const QString& category, int amount);

signals:
    void statusChanged();
    void transactionsModelChanged();

private:
    QString m_redisStatus;
    sw::redis::Redis m_redisClient;
    bool m_enableCaching;
};
