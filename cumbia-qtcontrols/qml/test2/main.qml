import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Extras 1.4
import CumbiaQmlControls 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    CircularGauge {
        id: circularGauge
        x: 118
        y: 164
    }


}
