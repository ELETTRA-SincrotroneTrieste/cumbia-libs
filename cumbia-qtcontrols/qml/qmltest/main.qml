import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.0
import QtCharts 2.1
import eu.elettra.cumbiaqmlcontrols 1.0

Window {
    id: window
    visible: true
    width: 1024
    height: 900
    title: qsTr("Hello World")


    CumbiaCircularGauge {
        id: circularGauge1
        x: 257
        y: 12
        source: "$1/double_scalar"
    }

    CumbiaSpectrumChart {
        id: spectrumChart
        x: 18
        y: 316
        sources: "$1/double_spectrum_ro"
    }

    CumbiaTrendChart {
        id: trendChart
        x: 18
        y: 709
        sources: "$1/double_scalar"
    }

    CumbiaLabel {
        id: cumbiaReadLabel
        x: 635
        y: 89
        source: "$1/double_scalar"
    }

}
