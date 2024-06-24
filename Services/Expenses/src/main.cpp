#include <drogon/drogon.h>

#include <nlohmann/json.hpp>

#include "ExpenseDbClient.h"
#include "JwtValidator.h"

using json = nlohmann::json;

int main() {
    drogon::app().addListener("0.0.0.0", 8081);

    auto dbClient = std::make_shared<ExpenseDbClient>("dbname=expensesdb user=postgres password=secret");

    drogon::app().registerHandler(
        "/add_expense",
        [dbClient](const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            std::string jwt = req->getHeader("Authorization").substr(7);  // Assumes 'Bearer ' is included

            if (!validateJwt(jwt, "YourPublicKeyHere")) {
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

            try {
                dbClient->addExpense(userId, amount, description);
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setBody("Expense added successfully");
                callback(resp);
            } catch (const std::exception &e) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k500InternalServerError);
                resp->setBody(std::string("Internal Server Error: ") + e.what());
                callback(resp);
            }
        },
        {drogon::Post});

    drogon::app().run();
}
