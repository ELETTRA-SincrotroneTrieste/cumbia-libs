import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.0
import eu.elettra.cumbiaqmlcontrols 1.0

CircularGauge {

    property alias source: backend.source

    QmlReaderBackend {
        id: backend
        source: ""
        Component.onCompleted: {
            init(cumbia_poof)
        }
    }

    id: circularGauge
    x: 67
    y: 42

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

            console.debug("color: ", color)
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

