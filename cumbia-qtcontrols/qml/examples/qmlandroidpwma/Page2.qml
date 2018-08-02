import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0



Item {
    id: secondPage

    Grid {
        id: grid
        columns: 4

        Text {
            id: text1
            text: qsTr("long_scalar")
            font.pixelSize: 12
        }

        CumbiaLabel {
            id: cumbiaReadLabel
            styleColor: "#0d99f2"
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
        }


        Text {
            id: text2
            text: qsTr("double_scalar")
            font.pixelSize: 12
        }
        CumbiaLabel {
            id: cumbiaReadLabel1
            styleColor: "#0d99f2"
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
        }


        Text {
            id: text3
            text: qsTr("long_scalar")
            font.pixelSize: 12
        }
        CumbiaCircularGauge {
            id:longCircularGauge
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
        }

        Text {
            id: text4
            text: qsTr("double_scalar")
            font.pixelSize: 12
        }
        CumbiaCircularGauge {
            id:doubleCircularGauge
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
        }

    }
}

