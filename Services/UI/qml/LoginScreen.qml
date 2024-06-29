import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import dcs.financemanager.style 1.0

Item {
    signal loginSuccessful
    signal registerRequested

    Rectangle {
        id: _rect
        anchors.fill: parent
        color: DCSStyles.colors.background
        Layout.alignment: Qt.AlignVCenter

        ColumnLayout  {
            anchors.centerIn: parent
            width: parent.width * 0.8
            spacing: 10

            Text {
                text: "Login"
                font: DCSStyles.titleFont
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: emailField
                placeholderText: "Email"
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                font: DCSStyles.bodyFont
            }

            TextField {
                id: passwordField
                placeholderText: "Password"
                echoMode: TextField.Password
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                font: DCSStyles.bodyFont
            }

            Button {
                text: "Login"
                Layout.fillWidth: true
                Material.background: Material.Blue
                font.family: DCSStyles.robotoBoldFont.name
                font.pixelSize: DCSStyles.fonts.medium
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    auth.loginUser(emailField.text, passwordField.text)
                }
            }

            Button {
                text: "Register"
                Layout.fillWidth: true
                font.family: DCSStyles.robotoBoldFont.name
                font.pixelSize: DCSStyles.fonts.medium

                Layout.alignment: Qt.AlignHCenter
                onClicked: registerRequested()
            }

            Button {
                text: "Forgot Password"
                Layout.fillWidth: true
                font.family: DCSStyles.robotoBoldFont.name
                font.pixelSize: DCSStyles.fonts.medium

                Layout.alignment: Qt.AlignHCenter
                onClicked: auth.sendPasswordResetEmail(emailField.text)
            }

            Text {
                id: statusText
                color: "red"
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: DCSStyles.fonts.medium
                wrapMode: Text.Wrap
                Layout.preferredWidth: parent.width
            }

            Connections {
                target: auth
                function onLoginStarted() {
                    busyIndicator.visible = true
                      overlay.visible = true
                }
                function onLoginSuccess(){
                    busyIndicator.visible = false
                    overlay.visible = false
                    statusText.color = "green"
                    statusText.text = "Login Successful";
                }
                function onLoginFailure(error){
                    busyIndicator.visible = false
                    overlay.visible = false
                    statusText.color = "red"
                    statusText.text = "Login Failed: " + error;
                }
                function onPasswordResetSuccess(){
                    statusText.color = "green"
                    statusText.text = "Password Reset Email Sent";
                }
                function onPasswordResetFailure(){
                    statusText.color = "red"
                    statusText.text = "Password Reset Failed: " + error;
                }
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
        running: visible
    }

    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "black"
        opacity: 0.25
        visible: false
        z: 1
        MouseArea {
            anchors.fill: parent
        }
    }
}
