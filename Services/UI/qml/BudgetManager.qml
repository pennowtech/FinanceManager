import QtQuick 2.15
import QtQuick.Controls 2.15

GroupBox {
    title: "Budget Manager"
    Layout.alignment: Qt.AlignTop
    Layout.fillHeight: true

    ColumnLayout {
        RowLayout {
            spacing: 10

            Label {
                text: "Category:"
            }

            ComboBox {
                id: budgetCategoryComboBox
                model: ["Food", "Transport", "Entertainment"]
            }
        }

        RowLayout {
            spacing: 10

            Label {
                text: "Budget:"
            }

            Slider {
                id: budgetSlider
                from: 0
                to: 1000
                stepSize: 10
            }
        }

        RowLayout {
            spacing: 10

            Button {
                text: "Set Budget"
                onClicked: {
                    financeManager.setBudget(budgetCategoryComboBox.currentText, budgetSlider.value)
                }
            }
        }

        Text {
            text: "Current Budget: " + budgetSlider.value
        }
    }
}
