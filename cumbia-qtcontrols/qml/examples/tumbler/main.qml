import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Extras 1.4

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")





        Tumbler {
            id: tumbler
            property int intDigits: 3
            property int decDigits: 2
            property double value: 0.0

            property double minimumValue: -255.0
            property double maximumValue: 255.0

            property var old_model: [ "+", 0, 0, 0, ".", 0 , 0 ]


            x: 165
            y: 159
            width: 121
            height: 78

            Component.onCompleted: {
                create()
                resizeColumns()
            }

            onWidthChanged: {
                resizeColumns()
            }

            onValueChanged: {
                console.log("onValueChanged")
            }

            onColumnChanged: {
                console.log("onColumnChanged: col", index, " from ", from, " to", to)
            }

            signal columnChanged(int index, int from, int to);

            function resizeColumns() {
                console.log("width", tumbler.width, "height ", tumbler.height)
                for(var i = 0; i < tumbler.columnCount; i++) {
                    var tcol = tumbler.getColumn(i)
                    tcol.width = tumbler.width / (intDigits + decDigits + 2)
                    if(i == 1 + intDigits)
                        tcol.width *= 0.3
                    console.log("col", i, " w ", tcol.width)
                }
            }

            function calculateValue() {
                var model = []
                for(var n = 0; n < tumbler.columnCount; n++) {
                    var acol = tumbler.getColumn(n)
                    model.push(acol.model[acol.currentIndex])
                }

                //
                // calculate value from the model instead of querying again all columns!
                //

                var val = 0.0;
                for(var i = 1; i < intDigits + 1; i++) {
                    var tcol = tumbler.getColumn(i)
                    var exp = intDigits - i;
                    console.log("value of model at i ", i, "exp", exp, "digit", tcol.model[tcol.currentIndex])
                    val += tcol.model[tcol.currentIndex] * Math.pow(10, exp)

                }

                var dexp = -1;
                for(var di = intDigits + 2; di < tumbler.columnCount; di++) {
                    var dcol = tumbler.getColumn(di)
                    console.log("value of model at i ", di, "exp", dexp, "digit", dcol.model[dcol.currentIndex])
                    val += dcol.model[dcol.currentIndex] * Math.pow(10, dexp)
                    dexp--
                }

                if(getColumn(0).currentIndex === 1)
                    val = -val

                for(var j = 0; j < model.length && old_model.length == model.length; j++) {
                    if(model[j] !== old_model[j]) {
                        var from = old_model[j]
                        var to = model[j]
                        tumbler.columnChanged(j, from, to)
                        break
                    }
                }

                old_model = model;

                console.log("CalculateValue", val)


            }

            function createConnections() {
                console.log("CreateConnections")
            }

            function create() {
                var number_model =  [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

                // plus / minus sign
                var pms = Qt.createQmlObject("import QtQuick.Extras 1.4; TumblerColumn {  }", tumbler)
                pms.model = ["+", "-"]
                tumbler.addColumn(pms)

                for(var i = 0; i < intDigits; i++){
                    var col = Qt.createQmlObject("import QtQuick.Extras 1.4; TumblerColumn {  }", tumbler)
                    col.model = number_model
                    col.width = tumbler.width/(intDigits + decDigits + 2)
                    tumbler.addColumn(col)
                }

                var commacol = Qt.createQmlObject("import QtQuick.Extras 1.4; TumblerColumn {}", tumbler)
                commacol.model = ["."]
                tumbler.addColumn(commacol)

                for(var decd = 0; decd < decDigits; decd++){
                    var decd_col = Qt.createQmlObject("import QtQuick.Extras 1.4; TumblerColumn { } ", tumbler)
                    decd_col.model = number_model
                    tumbler.addColumn(decd_col)
                }

                for(var tum = 0; tum < tumbler.columnCount; tum++) {
                    var tcol = getColumn(tum)
                    tcol.currentIndexChanged.connect(calculateValue)
                }
            }

        }

}
