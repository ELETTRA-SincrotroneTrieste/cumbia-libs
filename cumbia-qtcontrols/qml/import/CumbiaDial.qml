import QtQuick 2.0
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import eu.elettra.cumbiaqmlcontrols 1.0

Dial {

    property alias backend: w_backend
    property alias target: w_backend.target
    width: 200
    height: 200

    QmlWriterBackend {
        id: w_backend
        target: "$1/double_scalar"
        Component.onCompleted: {
            init(cumbia_poo_o)
        }
    }

    style: DialStyle {
        tickmarkStepSize: /*Math.round*/(dial.maximumValue - dial.minimumValue )/10
        labelStepSize: tickmarkStepSize
    }

    id: dial

    minimumValue: w_backend.min
    maximumValue: w_backend.max
    value: w_backend.value

    onValueChanged: {
        // hovered is not enough: if you launch the application and the mouse pointer is
        // over the control, hover is true!
        if(pressed) {
            w_backend.write(value)
        }
    }
}
