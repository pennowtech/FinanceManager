#pragma once

#include <QObject>
#include <QString>
#include <RedisDB.h>
#include <CassandraDB.h>
#include <memory>

class DBManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString cassandraStatus READ cassandraStatus NOTIFY statusChanged)
public:
    explicit DBManager(QObject *parent = nullptr);
    QString cassandraStatus() const;

    void setSettings(const QString &cassandraHost, bool enableCaching);
    Q_INVOKABLE void testConnections(const QString &cassandraHost);
    Q_INVOKABLE void applySettings(const QString &cassandraHost, bool enableCaching);

    void saveTransaction(const QString &amount, const QString &category);
    void loadTransactions();
    void setBudget(const QString &category, int amount);

signals:
    void statusChanged();
    void transactionsModelChanged();

private:
    std::shared_ptr<RedisDB> m_redisDB;
    std::shared_ptr<CassandraDB> m_cassandraDB;
};

