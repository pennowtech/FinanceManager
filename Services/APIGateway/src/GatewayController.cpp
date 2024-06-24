#include <GatewayController.h>

using namespace drogon;

void GatewayController::asyncHandleHttpRequest(
    const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)  {
    auto path = req->path();
    // Simplified routing
    if (path == "/login") {
        // Handle login, verify with Auth API
    } else if (path == "/register") {
        // Handle registration
    } else if (path == "/test")  {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("<p>Hello, world!</p>");
        resp->setExpiredTime(0);
        callback(resp);
    }
}
