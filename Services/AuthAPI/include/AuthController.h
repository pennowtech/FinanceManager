#include <drogon/drogon.h>
#include <drogon/HttpSimpleController.h>
#include <jwt-cpp/jwt.h>

class AuthController : public drogon::HttpSimpleController<AuthController> {
public:
    virtual void asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) override;

    PATH_LIST_BEGIN
    PATH_ADD("/auth/login", Post);
    PATH_ADD("/auth/register", Post);
    PATH_LIST_END
};

