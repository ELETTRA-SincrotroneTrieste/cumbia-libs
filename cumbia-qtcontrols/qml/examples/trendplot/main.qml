import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import eu.elettra.cumbiaqmlcontrols 1.0

Window {
    visible: true
    width: 800
    height: 480
    title: qsTr("Trend Plot Example")

    Plot {
        id: trendplot
        anchors.fill: parent

        sources: CmdLineUtils.sourcesFromArgs(CmdLineUtils.SrcType.Tango|CmdLineUtils.SrcType.Epics).join(";")
        plotYAutoscale: cbYAutoscale.checked
        cbYAutoscale.checked: trendChart.yAutoscale
        sbYMax.enabled: !cbYAutoscale.checked
        sbYMin.enabled: !cbYAutoscale.checked

        cbYAutoscale.onCheckedChanged: {
            console.log("onCbYAutoscale", cbYAutoscale.checked)
            if(!cbYAutoscale.checked){
                console.log("binding")
                plot.yMin = Qt.binding(function() { return sbYMin.value })
                plot.yMax = Qt.binding(function() { return sbYMax.value })
            }
        }

    }
}
