#include <Authentication.h>
#include <httplib.h>

#include <QDebug>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Authentication::Authentication(QObject *parent) : QObject(parent) {
    // Set your Firebase Web API Key here
    firebaseApiKey = "YOUR_FIREBASE_API_KEY";
}

void Authentication::registerUser(const QString &email, const QString &password) {
    std::string url = "<https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=>" + firebaseApiKey;
    json payload = {{"email", email.toStdString()}, {"password", password.toStdString()}, {"returnSecureToken", true}};

    std::string response = performPostRequest(url, payload.dump());
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("idToken")) {
        emit registrationSuccess();
    } else {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit registrationFailure(error);
    }
}

void Authentication::loginUser(const QString &email, const QString &password) {
    std::string url = "<https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=>" + firebaseApiKey;
    json payload = {{"email", email.toStdString()}, {"password", password.toStdString()}, {"returnSecureToken", true}};

    std::string response = performPostRequest(url, payload.dump());
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("idToken")) {
        emit loginSuccess();
    } else {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit loginFailure(error);
    }
}

void Authentication::sendPasswordResetEmail(const QString &email) {
    std::string url = "<https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode?key=>" + firebaseApiKey;
    json payload = {{"requestType", "PASSWORD_RESET"}, {"email", email.toStdString()}};

    std::string response = performPostRequest(url, payload.dump());
    auto jsonResponse = json::parse(response);

    if (jsonResponse.contains("email")) {
        emit passwordResetSuccess();
    } else {
        QString error = QString::fromStdString(jsonResponse["error"]["message"].get<std::string>());
        emit passwordResetFailure(error);
    }
}

std::string Authentication::performPostRequest(const std::string &url, const std::string &payload) {
    httplib::Client cli("<https://identitytoolkit.googleapis.com>");
    auto res = cli.Post(url.c_str(), payload, "application/json");

    if (res && res->status == 200) {
        return res->body;
    } else {
        return "{}";
    }
}
