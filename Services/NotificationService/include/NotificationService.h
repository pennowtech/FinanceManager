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
