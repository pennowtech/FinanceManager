import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 300; height: 300

    Column {
        spacing: 10
        anchors.centerIn: parent

        TextField {
            id: emailField
            placeholderText: "Enter email"
        }
        TextField {
            id: passwordField
            placeholderText: "Enter password"
            echoMode: TextField.Password
        }
        TextField {
            id: passwordFieldRepeat
            placeholderText: "Confirm password"
            echoMode: TextField.Password
        }
        Button {
            text: "Register"
            onClicked: {
                auth.registerUser(emailField.text, passwordField.text)
                loginSuccessful()
            }
        }
    }
}
