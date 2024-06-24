import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    signal loginSuccessful
    signal registerRequested

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        TextField {
            id: emailField
            placeholderText: "Email"
        }

        TextField {
            id: passwordField
            placeholderText: "Password"
            echoMode: TextField.Password
        }

        Button {
            text: "Login"
            onClicked: {
                auth.loginUser(emailField.text, passwordField.text)
                loginSuccessful()
            }
        }

        Button {
            text: "Register"
            onClicked: registerRequested()
        }
        Button {
            text: "Forgot Password"
            onClicked: auth.sendPasswordResetEmail(emailField.text)
        }
        Text {
            id: statusText
            color: "red"
        }
        Connections {
            target: auth
            onRegistrationSuccess: statusText.text = "Registration Successful"
            onRegistrationFailure: statusText.text = "Registration Failed: " + error
            onLoginSuccess: statusText.text = "Login Successful"
            onLoginFailure: statusText.text = "Login Failed: " + error
            onPasswordResetSuccess: statusText.text = "Password Reset Email Sent"
            onPasswordResetFailure: statusText.text = "Password Reset Failed: " + error
        }                
    }
}
