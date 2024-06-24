#include <drogon/WebSocketController.h>
#include <drogon/drogon.h>

class NotificationService : public drogon::WebSocketController<NotificationService>
{
public:
    virtual void handleNewMessage(const drogon::WebSocketConnectionPtr&, std::string&& message, const drogon::WebSocketMessageType& type) override;

    virtual void handleConnectionClosed(const drogon::WebSocketConnectionPtr&) override;

    WS_PATH_LIST_BEGIN
    // List WebSocket paths
    WS_PATH_ADD("/notification");
    WS_PATH_LIST_END
};
