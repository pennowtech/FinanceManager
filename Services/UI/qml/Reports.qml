import QtQuick 2.15
import QtQuick.Controls 2.15

GroupBox {
    title: "Reports"
    Layout.alignment: Qt.AlignTop
    Layout.fillHeight: true

    ColumnLayout {
        RowLayout {
            spacing: 10

            Label {
                text: "Report Type:"
            }

            ComboBox {
                id: reportTypeComboBox
                model: ["Monthly", "Yearly"]
            }
        }

        RowLayout {
            spacing: 10

            Button {
                text: "Generate Report"
                onClicked: {
                    financeManager.generateReport(reportTypeComboBox.currentText)
                }
            }
        }

        RowLayout {
            spacing: 10

            Button {
                text: "Save Report"
                onClicked: fileDialog.open()
            }
        }
    }
}
