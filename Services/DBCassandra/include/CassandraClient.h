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
