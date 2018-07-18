import QtQuick 2.6
import QtQuick.Controls 2.1
import eu.elettra.cumbiaqmlcontrols 1.0

CuButton {
    id: wbutton

    property alias target: w_backend.target
    property alias writer: w_backend

    property var value: null


    QmlWriterBackend {
        id: w_backend
        target: ""
        Component.onCompleted: {
            init(cumbia_poo_o)
        }
    }

    onClicked: {
        w_backend.write(value)
    }

}

