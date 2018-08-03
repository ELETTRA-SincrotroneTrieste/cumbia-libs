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
        sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/float_spectrum_ro"
        id: spectrumChart
        height: 450
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        title: "Tango Cumbia Spectrum Chart"
        backend.period: 10
    }

    CumbiaLabel {
        id: cumbiaReadLabel
        height: 91
        anchors.top: parent.top
        anchors.topMargin: 449
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: spectrumChart.left
        anchors.rightMargin: -280
        styleColor: "#0d99f2"
        source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
    }

    Connections {
        target: Qt.application
        onStateChanged: {
            switch (Qt.application.state) {
            case Qt.ApplicationSuspended:
            case Qt.ApplicationInactive:
            case Qt.ApplicationHidden:
                console.log("~~~~~~~~~~~~~~~~ App suspended")
                spectrumChart.backend.suspend()
                cumbiaReadLabel.backend.suspend()
                cu_websocket_client.close()
                break

            case Qt.ApplicationActive:
                console.log("++++++++++++++++ App ACTIVE")
                cu_websocket_client.open()
                spectrumChart.backend.start()
                cumbiaReadLabel.backend.start()
                break
            }
        }
    }

}
