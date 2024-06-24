#pragma once

#include <drogon/HttpSimpleController.h>
#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>

class GatewayController : public drogon::HttpSimpleController<GatewayController>
{
public:
    virtual void asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) override;

    PATH_LIST_BEGIN
    // List paths
    PATH_ADD("/login", drogon::Post);
    PATH_ADD("/register", drogon::Post);
    PATH_ADD("/test", drogon::Get);
    PATH_LIST_END
};
