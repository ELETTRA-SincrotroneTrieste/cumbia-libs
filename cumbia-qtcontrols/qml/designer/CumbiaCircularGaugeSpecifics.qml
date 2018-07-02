import QtQuick 2.1
import HelperWidgets 2.0
import QtQuick.Layouts 1.0

Column {
    anchors.left: parent.left
    anchors.right: parent.right

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: qsTr("CircularGauge")

        SectionLayout {
            Label {
                text: qsTr("Value")
                tooltip: qsTr("Value")
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.value
                    minimumValue: backendValues.minimumValue.value
                    maximumValue: backendValues.maximumValue.value
                }
                ExpandingSpacer {
                }
            }

            Label {
                text: qsTr("Minimum Value")
                tooltip: qsTr("Minimum Value")
            }
            SecondColumnLayout {
                SpinBox {
                    id: minimumValueSpinBox
                    backendValue: backendValues.minimumValue
                    minimumValue: 0
                    maximumValue: backendValues.maximumValue.value
                }
                ExpandingSpacer {
                }
            }

            Label {
                text: qsTr("Maximum Value")
                tooltip: qsTr("Maximum Value")
            }
            SecondColumnLayout {
                SpinBox {
                    id: maximumValueSpinBox
                    backendValue: backendValues.maximumValue
                    minimumValue: backendValues.minimumValue.value
                    maximumValue: 1000
                }
                ExpandingSpacer {
                }
            }

            Label {
                text: qsTr("Step Size")
                tooltip: qsTr("Step Size")
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.stepSize
                    minimumValue: 0
                    maximumValue: backendValues.maximumValue.value
                }
                ExpandingSpacer {
                }
            }


        }
    }

    Section {
        width: parent.width
        caption: qsTr("Cumbia reader")


        Label {
            text: qsTr("Source")
            tooltip: qsTr("The source of the reader, e.g. $1/double_scalar")
        }
        SecondColumnLayout {
            LineEdit {
                id: sourceLineEdit;
                backendValue: backendValues.source
                Layout.fillWidth: true
            }

        }
    }
}
