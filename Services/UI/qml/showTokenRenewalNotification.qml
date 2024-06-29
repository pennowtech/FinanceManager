// main.qml

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Application")

    NotificationDialog {
        id: notificationDialog
        visible: false
        anchors.centerIn: parent
        onOkClicked: {
            notificationDialog.visible = false
            auth.renewToken()
        }
        onCancelClicked: {
            notificationDialog.visible = false
            // Handle token expiration or any other necessary action
        }
    }

    function showTokenRenewalNotification() {
        notificationDialog.title = "Session Expiration"
        notificationDialog.message = "Your session is about to expire. Do you want to renew your session?"
        notificationDialog.okText = "Yes"
        notificationDialog.cancelText = "No"
        notificationDialog.visible = true
    }

    Rectangle {
        id: overlay
        visible: notificationDialog.visible
        anchors.fill: parent
        color: "transparent"
    }

    Button {
        text: "Simulate Token Renewal Request"
        onClicked: {
            showTokenRenewalNotification()
        }
        anchors.centerIn: parent
    }
}
