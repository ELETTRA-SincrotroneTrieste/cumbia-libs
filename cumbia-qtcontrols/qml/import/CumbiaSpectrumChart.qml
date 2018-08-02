import QtQuick 2.0
import QtCharts 2.1
import eu.elettra.cumbiaqmlcontrols 1.0

ChartView {

    id: spectrumChartView

    // geometry
    x: 0
    y: 0
    width: 800
    height: 400

    // properties
    property alias sources: sp_c_backend.sources
    property bool openGL: true
    property bool openGLSupported: true
    property alias yMax: spaxisY1.max
    property alias yMin: spaxisY1.min
    property alias backend: sp_c_backend

    // stores the current sources so that multiple calls to setSources
    // do not remove/readd already existing sources
    // if the application is suspended, and resumed repeatedly, this would
    // be an annoyance
    property var sourceslist: []

    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight
    title: "Spectrum Chart"

    // C++ backend
    QmlChartBackend {
        id: sp_c_backend
        sources: ""
        Component.onCompleted: {
            console.log("QmlChartBackend", "init")
            init(cumbia_poo_o)
        }
        onSourcesChanged: spectrumChartView.setSources(srcs)
        onNewSpectrum: spectrumChartView.refreshSpectrum(src)
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
        id: spaxisY1
        min: sp_c_backend.yMin  // auto scale
        max: sp_c_backend.yMax

        function autoscaleChanged() {
            if(sp_c_backend.yAutoscale) {
                min = sp_c_backend.yMin
                max = sp_c_backend.yMax
            }
        }
    }

    ValueAxis {
        id: spaxisY2
        min: 0
        max: 1000

        function autoscaleChanged() {
            if(sp_c_backend.yAutoscale) {
                min = sp_c_backend.yMin
                max = sp_c_backend.yMax
            }
        }
    }

    ValueAxis {
        id: spaxisX
        min: sp_c_backend.xMin
        max: sp_c_backend.xMax

        function autoscaleChanged() {
            if(sp_c_backend.xAutoscale) {
                min = sp_c_backend.xMin
                max = sp_c_backend.xMax
            }
        }
    }

    function setSources(srcs) {
//        spectrumChartView.removeAllSeries();

        for(var i = 0; i < srcs.length; i++) {
            console.log("set sources with", srcs, "index Of in ", sourceslist, "is ", sourceslist.indexOf(srcs[i]))
            if(sourceslist.indexOf(srcs[i]) < 0) {
                var series1 = spectrumChartView.createSeries(ChartView.SeriesTypeLine, srcs[i], spaxisX, spaxisY1);
                series1.width = 0
                series1.useOpenGL = spectrumChartView.openGL
                sourceslist.push(srcs[i])
            }
        }
    }

    function refreshSpectrum(src) {
        var seri = spectrumChartView.series(src)
        if(seri !== null) {
            sp_c_backend.replaceData(seri)
        }
    }

    function setAnimations(enabled) {
        if (enabled)
            trendChartView.animationOptions = ChartView.SeriesAnimations;
        else
            trendChartView.animationOptions = ChartView.NoAnimation;
    }
}
