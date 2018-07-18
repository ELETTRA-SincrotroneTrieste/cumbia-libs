import QtQuick 2.9
import QtQuick.Window 2.2
import eu.elettra.cumbiaqmlcontrols 1.0
import QtQuick.Extras 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")


    CumbiaDial {
        id: cumbiaDial
        target: "$1/double_scalar"
        x: 57

        y: 42
    }

    CuButton {
        id: simplestyledButton
        x: 398
        y: 134
    }

    CuTumbler {
        id: tumbler1
        target: "$1/double_scalar"
        x: 490
        y: 560
        anchors.rightMargin: 49
        anchors.bottomMargin: 0
        anchors.leftMargin: 219
        anchors.topMargin: 396
    }

    CuWButton {
        id: commandButton
        target: "$1/string_scalar"
        x: 456
        y: 276
        value:t_input_double.text

    }

    TextInput {
        id: t_input_double
        x: 333
        y: 276
        width: 117
        height: 40
        font.pixelSize: 12
        text:commandButton.writer.value
        horizontalAlignment: Text.AlignHCenter|Text.AlignVCenter
    }

    Text {
        id: text1
        x: 66
        y: 276
        width: 68
        height: 40
        text: qsTr("string_scalar")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
    }

    CumbiaLabel {
        id: cumbiaReadLabel
        x: 154
        y: 276
        width: 173
        height: 40
        fontSizeMode: Text.HorizontalFit
        source: "$1/string_scalar"
    }

    CumbiaLabel {
        id: cumbiaReadLabel1
        x: 159
        y: 322
        width: 168
        height: 42
        source: "$1/double_scalar"
    }

    TextInput {
        id: ti_double
        x: 333
        y: 328
        width: 127
        height: 36
        font.pixelSize: 12
        text:cmdButton.writer.value
    }

    CuWButton {
        id: cmdButton
        x: 456
        y: 324
        value:ti_double.text
        target: "$1/double_scalar"
    }

}
