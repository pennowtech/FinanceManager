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
#include <JwtAuthFilter.h>

#include <Utils/EnvReader.h>
#include <Utils/Logger.h>
#include <Utils/LoggerQuill.h>

#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

#include <map>

// Function to verify JWT token
bool verify_token(const std::string& projectId, const std::string& token, const std::unordered_map<std::string, std::string>& publicKeysMap)
{
    try
    {
        auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);

        // Extract the header to get the kid
        auto kid = decoded.get_header_claim("kid").as_string();

        // Find the corresponding public key
        if (publicKeysMap.find(kid) == publicKeysMap.end())
        {
            std::cerr << "kid (" << kid << ") not found.\n";
            return true;
        }
        auto pub_key = publicKeysMap.at(kid);

        auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
                            .allow_algorithm(jwt::algorithm::rs256{pub_key})
                            .with_issuer("https://securetoken.google.com/" + projectId); // Replace with your Firebase project ID

        verifier.verify(decoded);
        std::cerr << "Authorization token verified.\n";
        return true;
    }
    catch (const jwt::error::invalid_json_exception& e)
    {
        std::cerr << "Token parsing error: " << e.what() << "\n";
    }
    catch (const jwt::error::token_verification_exception& e)
    {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k401Unauthorized);
        response->setBody("Unauthorized: Invalid token");
        std::cerr << "Unauthorized: Invalid token. " << e.what() << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Handle other standard exceptions. " << e.what() << "\n";
    }
    return false;
}

void fetchFirebasePublicKeys(std::shared_ptr<DCS::Utils::Logger> logger,
                             const std::function<void(const std::unordered_map<std::string, std::string>&, bool)>& callback)
{
    // Firebase public keys URL
    auto client = drogon::HttpClient::newHttpClient("https://www.googleapis.com");

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setPath("/robot/v1/metadata/x509/securetoken@system.gserviceaccount.com");
    DCS_LOG_INFO(logger, "Sending request to Google API to fetch firebase public keys");

    client->sendRequest(req, [logger, callback](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
        DCS_LOG_INFO(logger, "2:");
        std::unordered_map<std::string, std::string> keys;
        bool success = false;
        std::cerr << "3: "
                  << "result: " << result << ", getStatusCode: " << static_cast<int>(response->getStatusCode()) << std::endl;

        if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::k200OK)
        {
            DCS_LOG_INFO(logger, "2: ");
            try
            {
                auto json_response = nlohmann::json::parse(response->getBody());
                for (auto it = json_response.begin(); it != json_response.end(); ++it)
                {
                    keys[it.key()] = it.value();
                }
                success = true;
            }
            catch (const std::exception& e)
            {
                DCS_LOG_INFO(logger, "Exception while parsing response: " << e.what());
            }
        }
        else
        {
            std::cerr << "Failed to fetch Firebase public keys: "
                      << (result == drogon::ReqResult::Ok ? response->getStatusCode() : static_cast<int>(result)) << std::endl;
        }

        callback(keys, success);
    });

    DCS_LOG_INFO(logger, "Request sent, awaiting response...");
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

namespace
{
    struct CmdOptions
    {
        std::string envFileName{};
        int port{};
    };

    CmdOptions parseCommandLineOptions(std::shared_ptr<DCS::Utils::Logger>& logger, int argc, char* argv[])
    {
        CmdOptions cmdOptions;

        try
        {
            cxxopts::Options options(argv[0], " - API Gateway Command line options");
            options.add_options()("e,env", "Environment file name (probably .env in your root)", cxxopts::value<std::string>())(
                "p,port", "Port number", cxxopts::value<int>())("h,help", "Print usage");

            auto result = options.parse(argc, argv);

            if (result.count("help"))
            {
                DCS_LOG_ERROR(logger, options.help());
                exit(1);
            }

            cmdOptions.envFileName = result["env"].as<std::string>();
            DCS_LOG_INFO(logger, "Using env file: " << cmdOptions.envFileName);
        }
        catch (const cxxopts::exceptions::exception& e)
        {
            DCS_LOG_ERROR(logger, "Error parsing options: " << e.what());
            exit(1);
        }
        return cmdOptions;
    }
} // namespace

int main(int argc, char* argv[])
{
    auto processName = std::filesystem::path(argv[0]).filename().string();
    std::shared_ptr<DCS::Utils::Logger> logger = std::make_shared<DCS::Utils::LoggerQuill>(processName, LogLevel::Info, true);

    CmdOptions cmdOptions = parseCommandLineOptions(logger, argc, argv);

    auto config = EnvReader(cmdOptions.envFileName);
    std::unordered_map<std::string, std::string> publicKeyMap;
    std::string projectId{config.getValueOrDefault<std::string>(processName, "DCS_FIREBASE_PROJECT_ID", "")};

    if (projectId.empty())
    {
        DCS_LOG_ERROR(logger, "No Project ID specified. Please specify DCS_FIREBASE_PROJECT_ID in your .env file.");
        return 1;
    }

    fetchFirebasePublicKeys(logger, [projectId, logger](const std::unordered_map<std::string, std::string>& keys, bool success) {
        if (success)
        {
            verify_token(projectId,
                         "eyJhbGciOiJSUzI1NiIsImtpZCI6Ijc5M2Y3N2Q0N2ViOTBiZjRiYTA5YjBiNWFkYzk2ODRlZTg1NzJlZTYiLCJ0eXAiOiJKV1QifQ."
                         "eyJpc3MiOiJodHRwczovL3NlY3VyZXRva2VuLmdvb2dsZS5jb20vZmluYW5jZW1hbmFnZXItNjE0N2MiLCJhdWQiOiJmaW5hbmNlbWFuYWdlci02MTQ3YyIsImF"
                         "1dGhfdGltZSI6MTcxOTYzOTE3NywidXNlcl9pZCI6IkFNeWdKZkNnYU9OZ2RwREFTMEFTcEtnbU9qVDIiLCJzdWIiOiJBTXlnSmZDZ2FPTmdkcERBUzBBU3BLZ2"
                         "1PalQyIiwiaWF0IjoxNzE5NjM5MTc3LCJleHAiOjE3MTk2NDI3NzcsImVtYWlsIjoidXNlckBleGFtcGxlLmNvbSIsImVtYWlsX3ZlcmlmaWVkIjpmYWxzZSwiZ"
                         "mlyZWJhc2UiOnsiaWRlbnRpdGllcyI6eyJlbWFpbCI6WyJ1c2VyQGV4YW1wbGUuY29tIl19LCJzaWduX2luX3Byb3ZpZGVyIjoicGFzc3dvcmQifX0."
                         "Dv1kxHLFC4-4DKADMf0djC3384NSITvEaWPcxkemNcQ3_Wm_Uw5CUvOkX5VqcU0eleIRZK_AE-"
                         "MhX5SURtnsydQYh40pZUVuuSe04fY5gnsIYZuOTiG9Zw5oCtjVQZtzL3pK7OG9Tlz77EnoEDg-Efnh6jY2Mjgmr5r_"
                         "j8gS0uINJawB4eatXvRSSdGi9Srtt3cs1uzcJCBH2wqzZncILjElPHmQfrjIM8USili_fajHQju6vjKix0VUiLuVz662mts_1ds5bQdGF1_"
                         "L7BsQI8UncdfZWXyhofZShcVUHe0cwb5Gfi0V_J_Wm_YJlcCgbh8oC-RcIFAXyHhce_zOhQ",
                         keys);
            for (const auto& [kid, key] : keys)
            {
                DCS_LOG_INFO(logger, "Key ID: " << kid);
            }
            // Store or use the keys as needed
        }
        else
        {
            DCS_LOG_INFO(logger, "Failed to fetch public keys");
            // Handle the error (e.g., retry, use cached keys, etc.)
        }
    });

#if 0
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

#endif
    drogon::app().run();
    return 0;
}
