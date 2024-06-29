import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Personal Finance Manager")

    NotificationDialog{
        id: _notificationDialog
    }

    StackView {
        id: _stackView
        anchors.fill: parent
        initialItem: LoginScreen {
            onLoginSuccessful: _stackView.push(Qt.resolvedUrl("MainScreen.qml"))
            onRegisterRequested: _stackView.push(Qt.resolvedUrl("RegisterScreen.qml"))
        }
    }

    Component.onCompleted: {
        _notificationDialog.message = "Welcome to Personal Finance Manager"
        _notificationDialog.open()
    }

    Connections {
        target: _stackView.currentItem
        function onCancelRegistration() {
            _stackView.pop()
        }
    }
}
