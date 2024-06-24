#ifndef FINANCEMANAGER_H
#define FINANCEMANAGER_H

#include <QObject>
#include "DBManager.h"

class FinanceManager : public QObject {
    Q_OBJECT
public:
    explicit FinanceManager(QObject *parent = nullptr);
    
    Q_INVOKABLE void addTransaction(const QString &amount, const QString &category);
    Q_INVOKABLE void setBudget(const QString &category, int amount);
    Q_INVOKABLE QVariantList loadTransactions();
    Q_INVOKABLE void generateReport(const QString &reportType);

    void setDatabaseManager(DatabaseManager* dbManager);

private:
    void addExpense(const std::string& jwt, const std::string& expenseData);

private:
    std::shared_ptr<DBManager> m_dbManager;
};

#endif // FINANCEMANAGER_H
