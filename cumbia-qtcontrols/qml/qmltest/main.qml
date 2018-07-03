import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.0
import CumbiaQmlControls 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    CumbiaCircularGauge {
        id: circularGauge
        anchors.fill: parent
        source: "$1/double_scalar"
    }




}
