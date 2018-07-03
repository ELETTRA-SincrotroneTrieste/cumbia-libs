import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0

Rectangle {


    property alias source: backend.source
    property alias label: text1
    property alias font: text1.font
    property alias verticalAlignment: text1.verticalAlignment
    property alias horizontalAlignment: text1.horizontalAlignment
    property alias text: text1.text
    property alias fontSizeMode: text1.fontSizeMode
    property alias styleColor: text1.styleColor
    property alias textColor: text1.color
    property alias textFormat: text1.textFormat


    QmlReaderBackend {
        id: backend
        source: ""
        Component.onCompleted: {
            init(cumbia_poo_o)
        }
    }

    id: rectangle
    x: 0
    y: 0
    width: 200
    height: 100
    // priority given to state color, if defined
    color: (backend.stateColor.length > 0) ? backend.stateColor : backend.qualityColor
    radius: 3
    border.color: (backend.qualityColor === "white") ? "green" : backend.qualityColor
    border.width: 1

    Text {

        id: text1
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        text: backend.value
        anchors.fill: parent
    }

}


