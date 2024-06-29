import QtQuick 2.15
import QtQuick.Controls 2.15

import dcs.financemanager.style 1.0

Item {
    signal cancelRegistration

    Rectangle {
        id: _rect
        anchors.fill: parent
        color: "#F0F0F0"

        ColumnLayout  {
            anchors.centerIn: parent
            width: parent.width * 0.8
            spacing: 10

            Text {
                text: "Register"
                font: DCSStyles.titleFont
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: emailField
                font: DCSStyles.bodyFont
                placeholderText: "Enter email"
                Layout.fillWidth: true
            }

            TextField {
                id: passwordField
                placeholderText: "Enter password"
                echoMode: TextField.Password
                Layout.fillWidth: true
                font: DCSStyles.bodyFont
            }

            TextField {
                id: passwordFieldRepeat
                placeholderText: "Confirm password"
                echoMode: TextField.Password
                Layout.fillWidth: true
                font: DCSStyles.bodyFont
            }

            Button {
                text: "Register"
                Layout.fillWidth: true
                font.family: DCSStyles.robotoBoldFont.name
                font.pixelSize: DCSStyles.fonts.medium

                onClicked: {
                    if (passwordField.text === passwordFieldRepeat.text) {
                        auth.registerUser(emailField.text, passwordField.text)
                    } else {
                        _notificationDialog.message = "Passwords don't match"
                        _notificationDialog.open()
                    }
                }
            }

            Button {
                text: "Cancel"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                font.family: DCSStyles.robotoBoldFont.name
                font.pixelSize: DCSStyles.fonts.medium

                onClicked: cancelRegistration()
            }

            Text {
                id: statusText
                color: "red"
                font.pixelSize: DCSStyles.fonts.medium
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Connections {
                target: auth
                function onRegistrationStarted() {
                    busyIndicator.visible = true
                    overlay.visible = true
                }
                function onRegistrationSuccess() {
                    busyIndicator.visible = false
                    overlay.visible = false
                    statusText.color = "green"
                    statusText.text = "Registration Successful"
                }
                function onRegistrationFailure(error) {
                    busyIndicator.visible = false
                    overlay.visible = false
                    statusText.color = "red"
                    statusText.text = "Registration Failed: " + error
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
