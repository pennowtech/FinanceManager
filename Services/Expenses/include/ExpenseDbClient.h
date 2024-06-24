#pragma once

#include <iostream>
#include <pqxx/pqxx>
#include <string>

class ExpenseDbClient
{
    pqxx::connection conn;

public:
    ExpenseDbClient(const std::string& connStr)
        : conn(connStr)
    {
    }

    void addExpense(const std::string& userId, double amount, const std::string& description)
    {
        pqxx::work txn(conn);
        try
        {
            txn.exec_params("INSERT INTO expenses(user_id, amount, description) VALUES ($1, $2, $3)", userId, amount, description);
            txn.commit();
        }
        catch (const pqxx::sql_error& e)
        {
            std::cerr << "SQL error: " << e.what() << std::endl;
            std::cerr << "Query was: " << e.query() << std::endl;
            throw;
        }
    }
};
