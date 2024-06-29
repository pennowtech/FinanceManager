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
#include "FinanceManager.h"

FinanceManager::FinanceManager(QObject* parent)
    : QObject(parent)
    , m_dbManager(nullptr)
{
}

void FinanceManager::addTransaction(const QString& amount, const QString& category)
{
    addExpense(jwt, expenseData);
}

void FinanceManager::addExpense(const std::string& jwt, const std::string& expenseData)
{
    auto client = drogon::HttpClient::newHttpClient("https://api.yourdomain.com");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::HttpMethod::Post);
    req->setPath("/add_expense");
    req->setBody(expenseData);
    req->addHeader("Authorization", "Bearer " + jwt);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);

    client->sendRequest(req, [](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::k200OK)
        {
            std::cout << "Expense added successfully: " << response->getBody() << std::endl;
        }
        else
        {
            std::cerr << "Failed to add expense: " << response->getReasonPhrase() << std::endl;
        }
    });
}

void FinanceManager::setBudget(const QString& category, int amount)
{
    if (m_dbManager)
    {
        m_dbManager->setBudget(category, amount);
    }
}

QVariantList FinanceManager::loadTransactions()
{
    QVariantList transactionList;
    if (m_dbManager)
    {
        auto transactions = m_dbManager->loadTransactions();
        for (const auto& transaction : transactions)
        {
            QVariantMap transactionMap;
            transactionMap["amount"] = transaction.first;
            transactionMap["category"] = transaction.second;
            transactionList.append(transactionMap);
        }
    }
    return transactionList;
}

void FinanceManager::generateReport(const QString& reportType)
{
    if (m_dbManager)
    {
        m_dbManager->loadTransactions(); // Just to simulate generating report by loading data
    }
}

void FinanceManager::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;
}
