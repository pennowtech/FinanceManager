#include <GatewayController.h>
#include <JwtAuthFilter.h>

#include <iostream>
#include <libenvpp/env.hpp>
#include <map>

void fetchFirebasePublicKeys(const std::function<void(const std::unordered_map<std::string, std::string>&)>& callback)
{
    auto client = drogon::HttpClient::newHttpClient("<https://www.googleapis.com>");
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setPath("/robot/v1/metadata/x509/securetoken@system.gserviceaccount.com");

    client->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        if (result == drogon::ReqResult::Ok)
        {
            auto keys = jwt::parse_x509(response->getBody());
            callback(keys);
        }
        else
        {
            std::cerr << "Failed to fetch Firebase public keys: " << response->getReasonPhrase() << std::endl;
            // Handle error or retry logic
        }
    });
}

void forwardRequestToExpensesService(const drogon::HttpRequestPtr& req,
                                     const drogon::HttpResponsePtr& resp,
                                     const std::function<void(const drogon::HttpResponsePtr&)>& callback)
{
    auto client = drogon::HttpClient::newHttpClient("<http://localhost:9000>"); // URL of your microservice
    client->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        if (result == drogon::ReqResult::Ok)
        {
            callback(response); // Forward the response from the microservice to the client
        }
        else
        {
            auto errorResponse = drogon::HttpResponse::newHttpResponse();
            errorResponse->setStatusCode(drogon::k500InternalServerError);
            errorResponse->setBody("Internal server error");
            callback(errorResponse);
        }
    });
}

int main(int argc, char* argv[])
{
    auto pre = env::prefix("DCS");

    const auto projectId = pre.register_variable<std::string>("FIREBASE_PROJECT_ID");
    std::unordered_map<std::string, std::string> publicKeyMap;

    std::cout << "Api Gateway Service App. Firebase Project ID: " << projectId << "\n";

    fetchFirebasePublicKeys([](const std::unordered_map<std::string, std::string>& keys) {
        // Store the keys or set up your JWT verifier with them
    });

    drogon::app().addListener("0.0.0.0", 8080);

    drogon::app().registerFilter("/add_expense/*", std::make_shared<JwtAuthFilter>(projectId, publicKeyMap));
    // Registering route for adding expenses
    drogon::app().registerHandler("/add_expense",
                                  [](const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                                      // Here we assume the token is valid, forward the request
                                      forwardRequestToExpensesService(req, resp, callback);
                                      callback(resp);
                                  },
                                  {drogon::Post}); // Ensure this matches the method expected by your API

    drogon::app().run();
    return 0;
}
