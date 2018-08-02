import QtQuick 2.0
import QtCharts 2.1
import eu.elettra.cumbiaqmlcontrols 1.0

ChartView {

    id: trendChartView

    // geometry
    x: 0
    y: 0
    width: 800
    height: 400

    // properties
    property alias sources: c_backend.sources
    property alias yAutoscale: c_backend.yAutoscale
    property alias xAutoscale: c_backend.xAutoscale
    property alias dateTimeXAxisFormat: axisX.format

    property bool openGL: false    // can be enabled if qt version > 5.12
    property bool openGLSupported: false  // can be enabled if qt version > 5.12

    property alias yMax: axisY1.max
    property alias yMin: axisY1.min

    property alias axisY: axisY1
    property alias axisY2: axisY2
    property alias axisX: axisX

    property alias backend: c_backend

    // stores the current sources so that multiple calls to setSources
    // do not remove/readd already existing sources
    // if the application is suspended, and resumed repeatedly, this would
    // be an annoyance
    property var sourceslist: []

    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight
    title: "Trend Chart"

    // C++ backend
    QmlChartBackend {
        id: c_backend
        sources: ""

        Component.onCompleted: {
            console.log("CumbiaTrendChart", "init with opengl false")
            init(cumbia_poo_o)
        }

        onSourcesChanged: trendChartView.setSources(srcs)
        onNewPoint: trendChartView.append(src)
        onXAutoscaleChanged: axisX.autoscaleChanged()
        onYAutoscaleChanged: axisY.autoscaleChanged()
    }

    onOpenGLChanged: {
        if (openGLSupported) {
            for(var i = 0; i < count(); i++) {
                console.log("using opengl on series ", series(i).title, "yes|no", openGL)
                series(i).useOpenGL = openGL
            }
        }
    }

    Component.onCompleted: {

    }

    ValueAxis {
        id: axisY1
        min: c_backend.yMin  // auto scale
        max: c_backend.yMax

        function autoscaleChanged() {
            if(c_backend.yAutoscale) {
                axisY1.min = Qt.binding(function() { return c_backend.yMin })
                axisY1.max = Qt.binding(function() { return c_backend.yMax })
            }
        }
    }

    ValueAxis {
        id: axisY2
        min: 0
        max: 1000

        function autoscaleChanged() {
            if(c_backend.yAutoscale) {
                min = Qt.binding(function() { return c_backend.yMin })
                max = Qt.binding(function() { return c_backend.yMax })
            }
        }
    }

    DateTimeAxis {
        id: axisX
        min: c_backend.t1
        max: c_backend.t2
        labelsAngle: 65
        format: "hh:mm:ss"

        function autoscaleChanged() {
            if(c_backend.xAutoscale) {
                min = Qt.binding(function() { return c_backend.t1 })
                max = Qt.binding(function() { return c_backend.t2 })
            }
        }
    }

    function setSources(srcs) {
    //    trendChartView.removeAllSeries();
        for(var i = 0; i < srcs.length; i++) {
            console.log("set sources with", srcs, "index Of in ", sourceslist, "is ", sourceslist.indexOf(srcs[i]))
            if(sourceslist.indexOf(srcs[i]) < 0) {
                var series1 = trendChartView.createSeries(ChartView.SeriesTypeLine, srcs[i], axisX, axisY1);
                series1.width = 2
                series1.useOpenGL = trendChartView.openGL
                sourceslist.push(srcs[i])
            }
        }
    }

    function append(src) {
        var seri = trendChartView.series(src)
        if(seri !== null) {
            c_backend.appendPoint(seri)
            console.log("append, verify y bounds", axisY1.min, axisY1.max)
        }
    }

    function setAnimations(enabled) {
        if (enabled)
            trendChartView.animationOptions = ChartView.SeriesAnimations;
        else
            trendChartView.animationOptions = ChartView.NoAnimation;
    }
}
