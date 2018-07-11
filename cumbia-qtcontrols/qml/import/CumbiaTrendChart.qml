import QtQuick 2.0
import QtCharts 2.1
import eu.elettra.cumbiaqmlcontrols 1.0

ChartView {

    QmlChartBackend {
        id: chartbackend
        sources: []
        Component.onCompleted: {
            init(cumbia_poo_o)
        }
    }


    id: chartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeDark
    property bool openGL: true
    property bool openGLSupported: true

    chartbackend.sourcesChanged: sourcesChanged(srcs)

    onOpenGLChanged: {
        if (openGLSupported) {
            series("signal 1").useOpenGL = openGL;
            series("signal 2").useOpenGL = openGL;
        }
    }
    Component.onCompleted: {
        if (!series("signal 1").useOpenGL) {
            openGLSupported = false
            openGL = false
        }
    }


    ValueAxis {
        id: axisY1
        min: -1
        max: 4
    }

    ValueAxis {
        id: axisY2
        min: -10
        max: 5
    }

    ValueAxis {
        id: axisX
        min: 0
        max: 1024
    }

    //    LineSeries {
    //        id: lineSeries1
    //        name: "signal 1"
    //        axisX: axisX
    //        axisY: axisY1
    //        useOpenGL: chartView.openGL
    //    }
    //    LineSeries {
    //        id: lineSeries2
    //        name: "signal 2"
    //        axisX: axisX
    //        axisYRight: axisY2
    //        useOpenGL: chartView.openGL
    //    }
    //![1]

    //![3]
    function setXMax() {

    }

    function setSources(srcs) {
        chartView.removeAllSeries();

        if (type == "line") {
            console.log("setSources", srcs);
            var series1 = chartView.createSeries(ChartView.SeriesTypeLine, "signal 1",
                                                 axisX, axisY1);
            series1.useOpenGL = chartView.openGL
        }
    }

    function createAxis(min, max) {
        // The following creates a ValueAxis object that can be then set as a x or y axis for a series
        return Qt.createQmlObject("import QtQuick 2.0; import QtCharts 2.0; ValueAxis { min: "
                                  + min + "; max: " + max + " }", chartView);
    }
    //![3]

    function setAnimations(enabled) {
        if (enabled)
            chartView.animationOptions = ChartView.SeriesAnimations;
        else
            chartView.animationOptions = ChartView.NoAnimation;
    }


}

