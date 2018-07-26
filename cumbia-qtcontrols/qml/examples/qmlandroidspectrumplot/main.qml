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
        backend.period: 10
    }

    Connections {
        target: Qt.application
        onStateChanged: {
            switch (Qt.application.state) {
            case Qt.ApplicationSuspended:
                console.log("~~~~~~~~~~~~~~~~ App suspended")
                spectrumChart.backend.suspend()
                break;
            case Qt.ApplicationHidden:
                console.log("---------------- App hidden")
                spectrumChart.backend.suspend()
                break;
            case Qt.ApplicationActive:
                console.log("++++++++++++++++ App ACTIVE")
                spectrumChart.backend.start()
                break
            case Qt.ApplicationInactive:
                console.log("++++++++++++++++ App INACTIVE")
                spectrumChart.backend.suspend()
                break
            }
        }
    }

}
