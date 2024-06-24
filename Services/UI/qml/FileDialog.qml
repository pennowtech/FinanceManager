import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1

FileDialog  {
    id: fileDialog
    title: "Select a file"
    folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    nameFilters: ["All Files (*)"]

    onAccepted: {
        console.log("Selected file: " + fileDialog.fileUrl)
    }
    onRejected: {
        console.log("File selection was canceled")
    }
}
