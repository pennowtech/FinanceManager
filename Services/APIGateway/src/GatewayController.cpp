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
#include <GatewayController.h>

using namespace drogon;

void GatewayController::asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto path = req->path();
    // Simplified routing
    if (path == "/login")
    {
        // Handle login, verify with Auth API
    }
    else if (path == "/register")
    {
        // Handle registration
    }
    else if (path == "/test")
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("<p>Hello, world!</p>");
        resp->setExpiredTime(0);
        callback(resp);
    }
}
