#pragma once


#include <QObject>
#include <QString>
#include <sw/redis++/redis++.h>
#include <memory>

class RedisDB : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString redisStatus READ redisStatus NOTIFY statusChanged)
public:
    explicit RedisDB(QObject *parent = nullptr);
    QString redisStatus() const;

    void setSettings(const QString &redisHost, bool enableCaching);
    Q_INVOKABLE void testConnections(const QString &redisHost);
    Q_INVOKABLE void applySettings(const QString &redisHost, bool enableCaching);

    void saveTransaction(const QString &amount, const QString &category);
    void loadTransactions();
    void setBudget(const QString &category, int amount);

signals:
    void statusChanged();
    void transactionsModelChanged();

private:
    QString m_redisStatus;
    sw::redis::Redis m_redisClient;
    bool m_enableCaching;
};

