import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: settingsWindow
    title: qsTr("Settings")
    flags: Qt.FramelessWindowHint
    color: "#f5f5f5"  // Light grey background for the window

    // Adjust window size based on its content
    width: dbSettngsWindow.implicitWidth + 20
    height: dbSettngsWindow.implicitHeight + 20

    GroupBox {
        id: dbSettngsWindow
        title: "Settings"
        anchors.fill: parent
        anchors.margins: 10
        Material.elevation: 2

        ColumnLayout {
            spacing: 20
            width: parent.width

            RowLayout {
                spacing: 10
                Label {
                    text: "Cassandra Host:"
                }
                TextField {
                    id: cassandraHostField
                    placeholderText: "Enter Cassandra host"
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                spacing: 10
                Label {
                    text: "Redis Host:"
                }
                TextField {
                    id: redisHostField
                    placeholderText: "Enter Redis host"
                    Layout.fillWidth: true
                }
            }

            CheckBox {
                id: enableCachingCheckBox
                text: "Enable Caching"
            }

            RowLayout {
                Button {
                    text: "Test"
                    onClicked: {
                        // Assume databaseManager is a valid object in the scope
                        databaseManager.testConnections(cassandraHostField.text, redisHostField.text)
                        // Assume messageDialog is a declared Dialog object in the scope
                        messageDialog.title = "Test Result"
                        messageDialog.text = "Cassandra: " + databaseManager.cassandraStatus + "\nRedis: " + databaseManager.redisStatus
                        messageDialog.open()
                    }
                }

                Button {
                    text: "Cancel"
                    onClicked: settingsWindow.close()
                }

                Button {
                    text: "Apply"
                    onClicked: {
                        databaseManager.applySettings(cassandraHostField.text, redisHostField.text, enableCachingCheckBox.checked)
                        settingsWindow.close()
                    }
                }
                spacing: 10
                Layout.fillWidth: true
            }
        }
    }

    // Add drop shadow to distinguish window
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "gray"
        border.width: 1
        radius: 5
    }
}
