#include "FinanceManager.h"

FinanceManager::FinanceManager(QObject *parent) : QObject(parent), m_dbManager(nullptr) {}

void FinanceManager::addTransaction(const QString &amount, const QString &category) {
    addExpense(jwt, expenseData);
}

void FinanceManager::addExpense(const std::string &jwt, const std::string &expenseData) {
    auto client = drogon::HttpClient::newHttpClient("https://api.yourdomain.com");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::HttpMethod::Post);
    req->setPath("/add_expense");
    req->setBody(expenseData);
    req->addHeader("Authorization", "Bearer " + jwt);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);

    client->sendRequest(req, [](drogon::ReqResult result, const drogon::HttpResponsePtr &response) {
        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::k200OK) {
            std::cout << "Expense added successfully: " << response->getBody() << std::endl;
        } else {
            std::cerr << "Failed to add expense: " << response->getReasonPhrase() << std::endl;
        }
    });
}

void FinanceManager::setBudget(const QString &category, int amount) {
    if (m_dbManager) {
        m_dbManager->setBudget(category, amount);
    }
}

QVariantList FinanceManager::loadTransactions() {
    QVariantList transactionList;
    if (m_dbManager) {
        auto transactions = m_dbManager->loadTransactions();
        for (const auto &transaction : transactions) {
            QVariantMap transactionMap;
            transactionMap["amount"] = transaction.first;
            transactionMap["category"] = transaction.second;
            transactionList.append(transactionMap);
        }
    }
    return transactionList;
}

void FinanceManager::generateReport(const QString &reportType) {
    if (m_dbManager) {
        m_dbManager->loadTransactions();  // Just to simulate generating report by loading data
    }
}

void FinanceManager::setDatabaseManager(DatabaseManager *dbManager) { m_dbManager = dbManager; }
