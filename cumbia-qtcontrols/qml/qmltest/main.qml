import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.0
import eu.elettra.cumbiaqmlcontrols 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    CircularGauge {

        QmlReaderBackend {
            id: cgbackend
            source: "$1/long_scalar"
            Component.onCompleted: {
                init(cumbia_poof)
            }
        }

        id: circularGauge
        x: 67
        y: 42
//        property QmlReaderBackend backend: cgbackend;

        maximumValue: cgbackend.max
        minimumValue: cgbackend.min
        style: CircularGaugeStyle {

            function get_color(val) {
                var color = "#000000"
                if(cgbackend.max_alarm !== cgbackend.min_alarm && (
                            val >= cgbackend.max_alarm || val <= cgbackend.min_alarm))
                    color = "red"
                else if(cgbackend.max_warning !== cgbackend.min_warning &&
                        (val >= cgbackend.max_warning || val <= cgbackend.min_warning) )
                    color = "orange"

                console.debug("color: ", color)
                return color
            }

            tickmarkStepSize: (circularGauge.maximumValue - circularGauge.minimumValue) / 10;

            tickmarkLabel:  Text {
                font.pixelSize: Math.max(6, outerRadius * 0.1)
                text: styleData.value
                color: get_color(styleData.value)
                antialiasing: true
            }
        }
        value: cgbackend.value
    }




}
