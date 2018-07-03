import QtQuick 2.1
import HelperWidgets 2.0
import QtQuick.Layouts 1.0

Column {
    width: parent.width

    StandardTextSection {
        width: parent.width
        showIsWrapping: true
        showFormatProperty: true
        showVerticalAlignment: true
    }

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: qsTr("Text Color")

        ColorEditor {
            caption: qsTr("Text Color")
            backendValue: backendValues.color
            supportGradient: false
        }
    }

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: qsTr("Style Color")

        ColorEditor {
            caption: qsTr("Style Color")
            backendValue: backendValues.styleColor
            supportGradient: false
        }
    }

    FontSection {
        width: parent.width
    }

    PaddingSection {
        width: parent.width
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
