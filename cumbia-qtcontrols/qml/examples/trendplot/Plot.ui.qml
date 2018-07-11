import QtQuick 2.4
import eu.elettra.cumbiaqmlcontrols 1.0
import QtQuick.Controls 2.3

Item {
    id: plotPage
    width: 600
    height: 400
    property alias sbYMax: sbYMax
    property alias sbYMin: sbYMin
    property alias sources: trendChart.sources
    property alias plot: trendChart
    property alias plotYAutoscale: trendChart.yAutoscale
    property alias cbYAutoscale: cbYAutoscale

    CumbiaTrendChart {
        id: trendChart
        anchors.bottomMargin: 53
        anchors.fill: parent
    }

    CheckBox {
        id: cbYAutoscale
        x: 20
        y: 353
        width: 170
        height: 40
        text: "Y Autoscale"
        anchors.right: label.left
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        checked: true
    }

    SpinBox {
        id: sbYMin
        x: 252
        y: 353
        anchors.right: labelYMax.left
        anchors.rightMargin: 22
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        editable: true
        from: -1000000
        to: 1000000
        value: 0
    }

    SpinBox {
        id: sbYMax
        x: 452
        y: 353
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        editable: true
        to: 1000000
        from: -1000000
        value: 1000
    }

    Label {
        id: label
        x: 196
        y: 353
        width: 50
        height: 39
        text: qsTr("Y Min")
        anchors.right: sbYMin.left
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: labelYMax
        x: 414
        y: 354
        width: 32
        height: 39
        text: qsTr("Y Max")
        anchors.right: sbYMax.left
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
