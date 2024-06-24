import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: messageDialog
    standardButtons: StandardButton.Ok
    modal: true
    onAccepted: {
        console.log("Dialog accepted")
    }

    onRejected: {
        console.log("Dialog rejected")
    }
    contentItem: Text {
        text: "This is a message dialog from Qt6 QML."
        padding: 20
    }
}
