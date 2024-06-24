#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace drogon;

class FirebaseAuth
{
public:
    FirebaseAuth();
    void registerUser(const std::string& email, const std::string& password);
    void loginUser(const std::string& email, const std::string& password, const std::function<void(const std::string&)>& callback);
    void sendPasswordResetEmail(const std::string& email);

private:
    std::string firebaseApiKey = "YOUR_FIREBASE_API_KEY";
    std::string performPostRequest(const std::string& url, const std::string& payload);
};
