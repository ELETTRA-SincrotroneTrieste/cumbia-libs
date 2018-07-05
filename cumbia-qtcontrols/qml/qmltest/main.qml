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

    CircularGauge {

        property alias source: backend.source

        QmlReaderBackend {
            id: backend
            source: "$1/double_scalar"
            Component.onCompleted: {
                console.log("CircularGauge.backend", "init")
                init(cumbia_poo_o)
            }
        }

        id: circularGauge
        x: 0
        y: 0
        width: 240
        height: 170

        maximumValue: backend.max
        minimumValue: backend.min
        style: CircularGaugeStyle {

            function get_color(val) {
                var color = "#000000"
                if(backend.max_alarm !== backend.min_alarm && (
                            val >= backend.max_alarm || val <= backend.min_alarm))
                    color = "red"
                else if(backend.max_warning !== backend.min_warning &&
                        (val >= backend.max_warning || val <= backend.min_warning) )
                    color = "orange"
                return color
            }

            tickmarkStepSize: Math.round((circularGauge.maximumValue - circularGauge.minimumValue) / 10);

            tickmarkLabel:  Text {
                font.pixelSize: Math.max(6, outerRadius * 0.1)
                text: styleData.value
                color: get_color(styleData.value)
                antialiasing: true
            }
        }
        value: backend.value
    }

    ChartView {

        id: trendChartView


        property bool openGL: false
        property bool openGLSupported: true

        property alias yMax: axisY1.max
        property alias yMin: axisY1.min

        property AbstractAxis axisX: null
        property bool isTrend: false

        QmlChartBackend {
            id: c_backend
            property var srcs: ["$1/double_scalar", "$2/double_scalar"]

            sources:  srcs

            Component.onCompleted: {
                console.log("QmlChartBackend", "init")
                init(cumbia_poo_o)
            }
            onSourcesChanged: trendChartView.setSources(srcs)
            onDataChanged: trendChartView.refresh(src, x, y)
        }

        property alias sources: c_backend.sources;
        x: 0
        y: 171
        width: 1024
        height: 335

        animationOptions: ChartView.NoAnimation
        theme: ChartView.ChartThemeLight
        title: "test chart"

        onOpenGLChanged: {
            if (openGLSupported) {

                series("signal 1").useOpenGL = openGL;
                series("signal 2").useOpenGL = openGL;
            }
        }
        Component.onCompleted: {
            //            if (!series("signal 1").useOpenGL) {
            //                openGLSupported = false
            //                openGL = false
            //            }
        }


        ValueAxis {
            id: axisY1
            min: c_backend.yMin  // auto scale
            max: c_backend.yMax
        }

        ValueAxis {
            id: axisY2
            min: 0
            max: 1000
        }


                DateTimeAxis {
                    id: axisX
                    min: c_backend.t1
                    max: c_backend.t2
                    labelsAngle: 65
                    format: "hh:mm:ss"
                }

        //    LineSeries {
        //        id: lineSeries1
        //        name: "signal 1"
        //        axisX: axisX
        //        axisY: axisY1
        //        useOpenGL: trendChartView.openGL
        //    }
        //    LineSeries {
        //        id: lineSeries2
        //        name: "signal 2"
        //        axisX: axisX
        //        axisYRight: axisY2
        //        useOpenGL: trendChartView.openGL
        //    }
        //![1]

        //![3]
        function setXMax() {

        }

        function setSources(srcs) {
            trendChartView.removeAllSeries();
            console.log("setSources", srcs, " length " , srcs.length)
            for(var i = 0; i < srcs.length; i++) {
                var series1 = trendChartView.createSeries(ChartView.SeriesTypeLine, srcs[i], axisX, axisY1);
                series1.useOpenGL = trendChartView.openGL
            }
        }

        function refresh(src, x, y) {
            var seri = trendChartView.series(src)
            if(seri !== null) {
                seri.append(x, y)
            }
        }


        //![3]

        function setAnimations(enabled) {
            if (enabled)
                trendChartView.animationOptions = ChartView.SeriesAnimations;
            else
                trendChartView.animationOptions = ChartView.NoAnimation;
        }



    }


    ChartView {

        id: spectrumChartView
        x: 0
        y: 277
        width: 1024
        height: 395
        anchors.verticalCenterOffset: 253
        anchors.verticalCenter: parent.verticalCenter


        property bool openGL: false
        property bool openGLSupported: true

        property alias yMax: axisY1.max
        property alias yMin: axisY1.min

        property AbstractAxis axisX: null
        property bool isTrend: false

        QmlChartBackend {
            id: sp_c_backend
            property var srcs: ["$1/double_spectrum_ro", "$2/double_spectrum_ro"]

            sources:  srcs

            Component.onCompleted: {
                console.log("QmlChartBackend", "init")
                init(cumbia_poo_o)
            }
            onSourcesChanged: spectrumChartView.setSources(srcs)
            onDataChanged: spectrumChartView.refresh(src, x, y)
            onSpectrumPointsChanged: spectrumChartView.refreshSpectrum(src)
        }

        property alias sources: sp_c_backend.sources;

        animationOptions: ChartView.NoAnimation
        theme: ChartView.ChartThemeLight
        title: "spectrum chart"

        onOpenGLChanged: {
            if (openGLSupported) {

                series("signal 1").useOpenGL = openGL;
                series("signal 2").useOpenGL = openGL;
            }
        }
        Component.onCompleted: {
            //            if (!series("signal 1").useOpenGL) {
            //                openGLSupported = false
            //                openGL = false
            //            }
        }


        ValueAxis {
            id: spaxisY1
            min: sp_c_backend.yMin  // auto scale
            max: sp_c_backend.yMax
        }

        ValueAxis {
            id: spaxisY2
            min: 0
            max: 1000
        }

        ValueAxis {
            id: spaxisX
            min: sp_c_backend.xMin
            max: sp_c_backend.xMax
        }

        function setSources(srcs) {
            spectrumChartView.removeAllSeries();
            console.log("setSources", srcs, " length " , srcs.length)
            for(var i = 0; i < srcs.length; i++) {
                var series1 = spectrumChartView.createSeries(ChartView.SeriesTypeLine, srcs[i], spaxisX, spaxisY1);
                series1.useOpenGL = spectrumChartView.openGL
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

}
