#pragma once

#include <QObject>
#include <string>

class Authentication : public QObject
{
    Q_OBJECT
public:
    explicit Authentication(QObject* parent = nullptr);

    Q_INVOKABLE void registerUser(const QString& email, const QString& password);
    Q_INVOKABLE void loginUser(const QString& email, const QString& password);
    Q_INVOKABLE void sendPasswordResetEmail(const QString& email);

signals:
    void registrationSuccess();
    void registrationFailure(const QString& error);
    void loginSuccess();
    void loginFailure(const QString& error);
    void passwordResetSuccess();
    void passwordResetFailure(const QString& error);

private:
    std::string performPostRequest(const std::string& url, const std::string& payload);
    std::string firebaseApiKey;
};
