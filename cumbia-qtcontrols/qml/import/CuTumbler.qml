import QtQuick 2.0
import QtQuick.Extras 1.4

Rectangle {
    width: 220
    height: 350
    color: "#494d53"
    border.width: 0

    ListModel {
        id: listModel

        ListElement {
            foo: "A"
            bar: "B"
            baz: "C"
        }
        ListElement {
            foo: "A"
            bar: "B"
            baz: "C"
        }
        ListElement {
            foo: "A"
            bar: "B"
            baz: "C"
        }
    }

    Tumbler {
        anchors.centerIn: parent

        TumblerColumn {
            model: listModel
            role: "foo"
        }
        TumblerColumn {
            model: listModel
            role: "bar"
        }
        TumblerColumn {
            model: listModel
            role: "baz"
        }
    }
}
