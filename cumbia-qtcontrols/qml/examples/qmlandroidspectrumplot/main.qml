import QtQuick 2.9
import QtQuick.Window 2.2
import eu.elettra.cumbiaqmlcontrols 1.0

Rectangle {
    visible: true
    width: 280
    height: 540
    id:window
//    title: qsTr("Hello World")

    CumbiaSpectrumChart {
        sources:  "websocket/spectrum1;websocket/spectrum2;websocket/spectrum3"
        id: spectrumChart
        title: "Random Cumbia Spectrum Chart"
        anchors.fill: parent
    }
}
