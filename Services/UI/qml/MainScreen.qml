import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    visible: true

    // Set Material Design theme
    Material.theme: Material.Light
    Material.accent: Material.Blue

    // Import custom components
    NotificationDialog {
        id: notificationDialog
    }

    MessageDialog {
        id: messageDialog
    }

    FileDialog {
        id: fileDialog
    }

    Settings {
        id: settingsWindow
    }

    MenuBar {
        id: menuBar
      
        Menu {
            title: "File"
            MenuItem {
                font.pixelSize: 14
                text: "Exit"
                onTriggered: Qt.quit()
                Label {
                    anchors.centerIn: parent
                    text: shortcutHelper2.nativeText
                    verticalAlignment: Text.AlignVCenter  
                    horizontalAlignment: Text.AlignRight  
                    anchors.right: parent.right
                }                  
                Shortcut {
                    id: shortcutHelper2
                    sequence: "Ctrl+Q"
                    onActivated: Qt.quit()
                }              
            }
        }
     
        Menu {
            title: "View"
            
            MenuItem {
                text: "Income/Expense"
                onTriggered: stackLayout.currentIndex = 0
            }
            MenuItem {
                text: "Budget Manager"
                onTriggered: stackLayout.currentIndex = 1
            }
            MenuItem {
                text: "Reports"
                onTriggered: stackLayout.currentIndex = 2
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: menuBar.height

        ToolBar {
            Layout.preferredHeight: 40
            spacing: 10
            Layout.fillWidth: true

            Flow {  // Ensures buttons are laid out in a row
                anchors.fill: parent

                ToolButton {
                    text: "Income/Expense"
                    icon.name: "document-open"
                    onClicked: stackLayout.currentIndex = 0
                    background: Rectangle {
                        color: mouseArea.containsMouse ? "#d3d3d3" : "transparent"
                        radius: 5
                    }
                    padding: 10

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }                    
                }
                ToolButton {
                    text: "Budget Manager"
                    onClicked: stackLayout.currentIndex = 1
                }
                ToolButton {
                    text: "Reports"
                    onClicked: stackLayout.currentIndex = 2
                }
                ToolButton {
                    text: "Settings"
                    onClicked: settingsWindow.visible = true
                }                
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Income and Expense Tracker Section
            IncomeExpenseTracker {
                Layout.alignment: Qt.AlignTop
            }

            // Budget Manager Section
            BudgetManager {
                Layout.alignment: Qt.AlignTop
            }

            // Reports Section
            Reports {
                Layout.alignment: Qt.AlignTop
            }
        }

        // Status Bar
        StatusBar {
            Layout.alignment: Qt.AlignTop
        }

        // Open Settings Button
        Button {
            text: "Settings"
            onClicked: settingsWindow.visible = true
        }
    }
}
