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
#include <drogon/drogon.h>

#include <nlohmann/json.hpp>

#include "ExpenseDbClient.h"
#include "JwtValidator.h"

using json = nlohmann::json;

int main()
{
    drogon::app().addListener("0.0.0.0", 8081);

    auto dbClient = std::make_shared<ExpenseDbClient>("dbname=expensesdb user=postgres password=secret");

    drogon::app().registerHandler("/add_expense",
                                  [dbClient](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                                      std::string jwt = req->getHeader("Authorization").substr(7); // Assumes 'Bearer ' is included

                                      if (!validateJwt(jwt, "YourPublicKeyHere"))
                                      {
                                          auto resp = drogon::HttpResponse::newHttpResponse();
                                          resp->setStatusCode(drogon::k401Unauthorized);
                                          callback(resp);
                                          return;
                                      }

                                      // Parse request data using nlohmann/json
                                      auto jsonReq = json::parse(req->getBody());
                                      std::string userId = jsonReq["userId"];
                                      double amount = jsonReq["amount"];
                                      std::string description = jsonReq["description"];

                                      try
                                      {
                                          dbClient->addExpense(userId, amount, description);
                                          auto resp = drogon::HttpResponse::newHttpResponse();
                                          resp->setBody("Expense added successfully");
                                          callback(resp);
                                      }
                                      catch (const std::exception& e)
                                      {
                                          auto resp = drogon::HttpResponse::newHttpResponse();
                                          resp->setStatusCode(drogon::k500InternalServerError);
                                          resp->setBody(std::string("Internal Server Error: ") + e.what());
                                          callback(resp);
                                      }
                                  },
                                  {drogon::Post});

    drogon::app().run();
}
