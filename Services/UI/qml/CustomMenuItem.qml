import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


// TODO: Not working right now
// Usage:
// Menu {
//     title: "File"
//     CustomMenuItem  {
//         text: "Exit"
//         shortcut: "Ctrl+Q"
//         onTriggered: Qt.quit
//     }
// }

Item {
    property string text: ""
    property string shortcut: ""
    property var onTriggered: {}

    height: 40
    width: parent.width

    RowLayout {
        anchors.fill: parent
        spacing: 10

        Text {
            id: textItem
            text: parent.text
            color:"#000"
        }

        Text {
            text: parent.shortcut
            color: "#888"  // Lighter color for the shortcut key
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (parent.onTriggered) {  // Check if the function is defined
                parent.onTriggered()
            }
        }
    }

    Component.onCompleted: {
        console.warn("Shortcut is not defined for", textItem.implicitHeight)
        if (!shortcut) {
            console.warn("Shortcut is not defined for", text)
        }
        if (!onTriggered) {
            console.warn("onTriggered action is not defined for", text)
        }
    }

    Shortcut {
        sequence: parent.shortcut
        context: Qt.ApplicationShortcut
        enabled: shortcut !== ""  // Only enable Shortcut if a sequence is provided
        onActivated: {
            if (onTriggered) {
                onTriggered()
            }
        }
    }
}
