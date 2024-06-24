import QtQuick 2.15
import QtQuick.Controls 2.15

GroupBox {
    title: "Income and Expense Tracker"
    Layout.alignment: Qt.AlignTop
    Layout.fillHeight: true

    ContextMenu {
        id: contextMenu
    }

    ColumnLayout {
        RowLayout {
            spacing: 10

            Label {
                text: "Amount:"
            }

            TextField {
                id: amountField
                placeholderText: "Enter amount"
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onPressed: function(mouse){
                        if (mouse.button === Qt.RightButton) {
                            contextMenu.parentField = amountField
                            contextMenu.x = mouse.x
                            contextMenu.y = mouse.y
                            contextMenu.open()
                        }
                    }
                }
            }

            Label {
                text: "Category:"
            }

            ComboBox {
                id: categoryComboBox
                model: ["Income", "Expense"]
            }
        }

        RowLayout {
            spacing: 10

            Button {
                text: "Add Transaction"
                onClicked: {
                    financeManager.addTransaction(amountField.text, categoryComboBox.currentText)
                }
            }
        }

        ListView {
            id: transactionList
            model: databaseManager.transactionsModel

            delegate: Item {
                width: parent.width
                RowLayout {
                    Text {
                        text: model.amount
                    }
                    Text {
                        text: model.category
                    }
                }
            }
        }
    }
}
