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
