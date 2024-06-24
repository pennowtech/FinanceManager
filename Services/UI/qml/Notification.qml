import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: notificationDialog
    modal: true
    focus: true
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2

    property string message: ""
    property int duration: 3000

    contentItem: Column {
        spacing: 10
        padding: 20

        Text {
            text: notificationDialog.message
        }
    }

    Timer {
        interval: notificationDialog.duration
        running: true
        repeat: true
        onTriggered: notificationDialog.close()
    }
}
