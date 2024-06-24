import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Personal Finance Manager")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: LoginScreen {
            onLoginSuccessful: stackView.push(Qt.resolvedUrl("MainScreen.qml"))
            onRegisterRequested: stackView.push(Qt.resolvedUrl("RegisterScreen.qml"))
        }
    }
}
