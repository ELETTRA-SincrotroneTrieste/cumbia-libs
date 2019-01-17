import QtQuick 2.9
import QtQuick.Window 2.2
import eu.elettra.cumbiaqmlcontrols 1.0
import QtQuick.Extras 1.4

Window {
    visible: true
    width: 640
    height: 980
    title: qsTr("Hello World")


    CumbiaDial {
        id: cumbiaDial
        target: "$1/double_scalar"
        x: 39

        y: 780
        width: 131
        height: 127
    }

    CuTumbler {
        id: tumbler1
        target: "$1/double_scalar"
        x: 490
        y: 560
        anchors.rightMargin: 50
        anchors.bottomMargin: 36
        anchors.leftMargin: 218
        anchors.topMargin: 780
    }

    CuWButton {
        id: commandButton
        target: "$1/string_scalar"
        x: 450
        y: 686
        value:t_input_double.text

    }

    TextInput {
        id: t_input_double
        x: 327
        y: 680
        width: 117
        height: 40
        font.pixelSize: 12
        text:commandButton.writer.value
        horizontalAlignment: Text.AlignHCenter|Text.AlignVCenter
    }

    Text {
        id: text1
        x: 58
        y: 682
        width: 68
        height: 40
        text: qsTr("string_scalar")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
    }

    CumbiaLabel {
        id: cumbiaStringReadLabel
        x: 148
        y: 680
        width: 173
        height: 40
        fontSizeMode: Text.HorizontalFit
        source: "$1/string_scalar"
    }

    CumbiaLabel {
        id: cumbiaDoubleReadLabel
        x: 148
        y: 623
        width: 173
        height: 42
        source: "$1/double_scalar"
    }

    TextInput {
        id: ti_double
        x: 327
        y: 732
        width: 127
        height: 36
        font.pixelSize: 12
        text:cmdButton.writer.value
    }

    CuWButton {
        id: cmdButton
        x: 450
        y: 734
        value:ti_double.text
        target: "$1/double_scalar"
    }

    CumbiaTrendChart {
        id: trendChart
        x: 11
        y: 18
        width: 614
        height: 273
        sources: "$1/double_scalar;$1/long_scalar"

        CumbiaCircularGauge {
            id: circularGauge
            x: 0
            y: 298
        }
    }

    Text {
        id: text2
        x: 39
        y: 625
        width: 68
        height: 40
        text: qsTr("double_scalar")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
    }

    Text {
        id: text3
        x: 342
        y: 625
        width: 68
        height: 40
        text: qsTr("long_scalar")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
    }

    CumbiaLabel {
        id: cumbiaLongReadLabel
        x: 446
        y: 623
        width: 173
        height: 42
        source: "$1/long_scalar"
    }


    Connections {
        target: Qt.application
        onStateChanged: {
            switch (Qt.application.state) {
            case Qt.ApplicationSuspended:
            case Qt.ApplicationHidden:
            case Qt.ApplicationInactive:
                console.log("~~~~~~~~~~~~~~~~ App suspended/hidden/inactive")
                cumbiaStringReadLabel.backend.suspend()
                cumbiaLongReadLabel.backend.suspend()
                cumbiaDoubleReadLabel.backend.suspend()
                trendChart.backend.suspend()
                break
            case Qt.ApplicationActive:
                console.log("++++++++++++++++ App ACTIVE")
                cumbiaStringReadLabel.backend.start()
                cumbiaLongReadLabel.backend.start()
                cumbiaDoubleReadLabel.backend.start()
                trendChart.backend.start()
                break
            }
        }
    }

    CumbiaDial {
        id: dial
        x: 320
        y: 306
        width: 159
        height: 151
    }

}
