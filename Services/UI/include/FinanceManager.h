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
#ifndef FINANCEMANAGER_H
#define FINANCEMANAGER_H

#include "DBManager.h"
#include <QObject>

class FinanceManager : public QObject
{
    Q_OBJECT
public:
    explicit FinanceManager(QObject* parent = nullptr);

    Q_INVOKABLE void addTransaction(const QString& amount, const QString& category);
    Q_INVOKABLE void setBudget(const QString& category, int amount);
    Q_INVOKABLE QVariantList loadTransactions();
    Q_INVOKABLE void generateReport(const QString& reportType);

    void setDatabaseManager(DatabaseManager* dbManager);

private:
    void addExpense(const std::string& jwt, const std::string& expenseData);

private:
    std::shared_ptr<DBManager> m_dbManager;
};

#endif // FINANCEMANAGER_H
