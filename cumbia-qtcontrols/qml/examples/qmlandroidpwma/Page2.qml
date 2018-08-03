import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ScrollView {
    id: secondPage
    width:300
    height:300
    clip: true

    ColumnLayout {
        id: grid
        spacing: 6

        Text {
            id: text1
            text: qsTr("long_scalar")
            font.pixelSize: 12
            width:parent.width
            Layout.margins: 10
        }

        CumbiaLabel {
            id: cumbiaReadLabel
            styleColor: "#0d99f2"
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }
        CumbiaCircularGauge {
            id:longCircularGauge
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }


        Text {
            id: text2
            text: qsTr("double_scalar")
            font.pixelSize: 12
            Layout.fillWidth: true
            Layout.margins: 10
        }
        CumbiaLabel {
            id: cumbiaReadLabel1
            styleColor: "#0d99f2"
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }

        CumbiaCircularGauge {
            id:doubleCircularGauge
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }

    }
}

