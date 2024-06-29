// NotificationDialog.qml

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: messageDialog
    property alias title: titleLabel.text
    property alias message: messageLabel.text
    property alias okText: okLabel.text
    property alias cancelText: cancelLabel.text
    signal okClicked
    signal cancelClicked

    Rectangle {
        id: dialog
        width: 400
        height: messageLabel.height + 100
        radius: 10
        color: "white"
        border.color: "black"
        border.width: 1
        anchors.centerIn: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            padding: 20

            Text {
                id: titleLabel
                text: "Notification"
                font.bold: true
                font.pointSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "black"
            }

            Text {
                id: messageLabel
                text: ""
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
                Layout.preferredWidth: 300
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "black"
            }

            RowLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                Layout.fillWidth: true

                Text {
                    id: okLabel
                    text: "Ok"
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    Layout.fillWidth: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            messageDialog.okClicked()
                        }
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onEntered: parent.color = "#dddddd"
                        onExited: parent.color = "transparent"
                    }
                }

                Rectangle {
                    width: 1
                    color: "black"
                    height: okLabel.height
                }

                Text {
                    id: cancelLabel
                    text: "Cancel"
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    Layout.fillWidth: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            messageDialog.cancelClicked()
                        }
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onEntered: parent.color = "#dddddd"
                        onExited: parent.color = "transparent"
                    }
                }
            }
        }
    }
}
