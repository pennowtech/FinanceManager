import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: contextMenu
    Layout.fillWidth: true
    // width: 150
    // height: column.height
    property TextField parentField: null

    Column {
        id: column
        // width: parent.width

        MenuItem {
            text: "Cut"
            enabled: parentField && parentField.activeFocus && parentField.selectedText.length > 0
            onTriggered: {
                parentField.cut();
                contextMenu.close();  // Explicitly close the menu
            }
        }

        MenuItem {
            text: "Copy"
            enabled: parentField && parentField.selectedText.length > 0
            onTriggered: {
                parentField.copy();
                contextMenu.close();  // Explicitly close the menu
            }
        }

        MenuItem {
            text: "Paste"
            enabled: parentField && parentField.activeFocus
            onTriggered: {
                parentField.paste();
                contextMenu.close();  // Explicitly close the menu
            }
        }
    }

}
