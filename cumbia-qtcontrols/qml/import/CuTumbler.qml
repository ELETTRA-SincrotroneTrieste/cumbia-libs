import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import eu.elettra.cumbiaqmlcontrols 1.0

Rectangle {
    id: container

    property alias target: w_backend.target

    property int intDigits: 3
    property int decDigits: 2

    property double value: w_backend.value
    property double minimumValue: w_backend.min
    property double maximumValue: w_backend.max

    property var nmodel: []  // mirrors the value displayed by the CuTumbler
    property var columns: [] // tumblers

    property bool disableValueChanged: false

    property string applyButtonText: "APPLY"

    property Button applybutton: null

    signal columnChanged(int index, int from, int to);

    anchors.right: parent.right
    anchors.rightMargin: 268
    anchors.left: parent.left
    anchors.leftMargin: 0
    anchors.top: parent.top
    anchors.topMargin: 48
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 348
    border.width: 2

    radius: 5
    border.color: "#2ead09"

    Component.onCompleted: {
        create()
    }

    onValueChanged: {
        if(!disableValueChanged) {
            console.log("--- onValueChanged: ", value)
            var amodel = getNumberModel(value)
            updateTumblerModels(amodel) // disconnects and reconnects tumbler signal
            setTumblerValue(amodel) // disconnects and reconnects tumbler signal
            nmodel = amodel
        }
    }

    onMaximumValueChanged: {
        console.log("--- onMaximumValueChanged: max", maximumValue, " min ", minimumValue, "value", value)
        nmodel = adjustToBounds(nmodel)
        updateTumblerModels(nmodel)
    }

    onMinimumValueChanged: {
        console.log("--- onMinimumValueChanged: max", maximumValue, " min ", minimumValue, "value", value)
        nmodel = adjustToBounds(nmodel)
        updateTumblerModels(nmodel)
    }

    onIntDigitsChanged: {
        create()
        setValue(value)
    }

    onDecDigitsChanged: {
        create()
        setValue(value)
    }

    onApplyButtonTextChanged: {
        if(applybutton != null)
            applybutton.text = applyButtonText
    }

    QmlWriterBackend {
        id: w_backend
        target: ""
        Component.onCompleted: {
            init(cumbia_poo_o)
        }
    }


    RowLayout {
        id: row_layout
        anchors.fill: parent
    }


    onColumnChanged: {

        if(index == 0) {
            nmodel = adjustToBounds(nmodel)
        }

        updateTumblerModels(nmodel)

        var prevDigitIdx = getIndexLeft(index)
        var tcol
        var newval = null
        if(prevDigitIdx > 0) {
            if(from == 9 && to == 0) {
                //                    console.log("onColumnChanged: INCREMENTING idx" , prevDigitIdx, " col", index, " from ", from, " to", to)
                //incrementTumbler(prevDigitIdx)
                console.log("9 --> 0 new value would be", value + Math.pow(10, columnExp(index)))
                newval = value + Math.pow(10, columnExp(index))
            }
            //                else if(fromidx === 0 && toidx === tcol.model.length - 1) {
            else if (from === 0 && to === 9) {
                //                    console.log("onColumnChanged: DECREMENTING idx", prevDigitIdx, " col", index, " from ", from, " to", to, "from idx", fromidx, "to idx ", toidx)
                //decrementTumbler(prevDigitIdx)
                console.log("0 --> 9 new value would be", value - Math.pow(10, columnExp(index)))
                newval = value - Math.pow(10, columnExp(index))
            }
        }
        else
            console.log("index left of ", index, "is", prevDigitIdx)

        if(newval == null)
            nmodel = getModel()
        else
            nmodel = getNumberModel(newval)
        disableValueChanged = true;
        console.log("calling toValue from model ", nmodel)
        value = toValue(nmodel)
        setTumblerValue(nmodel)
        disableValueChanged = false;
    }

    function incrementTumbler(column) {
        var tc = columns[column]
        var val = tc.model[tc.currentIndex]
        var next_val
        if(val === 9)
            next_val = 0
        else
            next_val = val + 1
        var idx = tc.model.indexOf(next_val)
        if(idx > -1) {
            console.log("incrementing tumbler", column, "from idx", tc.currentIndex,
                        tc.model[tc.currentIndex], "to idx", idx, " value ", tc.model[idx])
            tc.currentIndex = idx
        }
    }

    function decrementTumbler(column) {
        var tc = columns[column]
        var val = tc.model[tc.currentIndex]

        var prev_val
        if(val === 0)
            prev_val = 9
        else
            prev_val = val - 1
        var idx = tc.model.indexOf(prev_val)
        if(idx > -1) {
            console.log("decrementing tumbler", column, "from idx", tc.currentIndex,
                        tc.model[tc.currentIndex], "to idx", idx, " value ", tc.model[idx])
            tc.currentIndex = idx
        }
        else
            console.log("decrementing tumbler: IDX IS -1!, model", model, "looking for prev val", prev_val)
        console.log("tc.currentIndex", tc.currentIndex, "model", tc.model)
    }

    function columnExp(column) {
        if(column <= intDigits)
            return intDigits - column
        else if(column === intDigits + 1) { // point!
            console.log("columnExp: error: column ", column, " is not valid")
            return pow(10, -1000000);
        }
        else
            return -(column - (intDigits + 2) + 1)
    }

    function getIndexLeft(index) {
        if(index === intDigits + 2) // digit following "."
            return intDigits
        return index - 1
    }

    function getIndexRight(index) {
        if(index === intDigits) // last integer digit
            return index + 2
        if(index === columns.length || index === 0)
            return -1
        return index + 1
    }

    function connectTumbler() {
        for(var c = 0; c < columns.length; c++) {
            var tcol = columns[c]
            tcol.currentIndexChanged.connect(tumblerColumnChanged)
        }
    }

    function disconnectTumbler() {
        for(var c = 0; c < columns.length; c++) {
            var tcol = columns[c]
            tcol.currentIndexChanged.disconnect(tumblerColumnChanged)
        }
    }

    function apply() {
        w_backend.write(value)
    }

    function setValue(val) {
        if(val >= minimumValue && val <= maximumValue && val !== value) {
            var amodel = getNumberModel(val)
            updateTumblerModels(amodel) // disconnects and reconnects tumbler signal
            setTumblerValue(amodel) // disconnects and reconnects tumbler signal
            nmodel = amodel
            value = val
        }
    }

    // called from setValue
    //
    function setTumblerValue(amodel) {
        var tc // tumbler column
        disconnectTumbler()
        console.log("setTumblerValue from model", amodel)
        for(var i = 0; i < columns.length; i++) {
            tc = columns[i]
            console.log("setting gurrent index from amodel[", i, "] which is value", amodel[i], "and index",  tc.model.indexOf(amodel[i]))
            tc.currentIndex=tc.model.indexOf(amodel[i])
        }
        connectTumbler()
    }

    // called from setValue
    //
    function getNumberModel(val) {

        var mo = []
        if(val >= 0)
            mo.push("+")
        else
            mo.push("-")

        if(val <= maximumValue && val >= minimumValue) {
            if(val < 0)
                val = -val
            var integer_part = Math.floor(val)
            var decimal_part = val - integer_part
            var dec_part_as_int = Math.floor(decimal_part * Math.pow(10, decDigits))

            console.log("decimal part", decimal_part, "as int", dec_part_as_int)

            var i
            var digit
            var div
            for(i = 0; i < intDigits; i++) {
                div = Math.pow(10, intDigits - i - 1)
                digit = Math.floor(integer_part / div)
                integer_part -= digit * div
                mo[i + 1] = digit
            }
            mo[i + 1] = "."

            for(i = 0; i < decDigits; i++) {
                div = Math.pow(10, decDigits - i - 1)
                digit = Math.floor(dec_part_as_int / div)
                dec_part_as_int -= digit * div

                mo[intDigits + 2 + i] = digit
            }
        }
        else {
            console.log("CumbiaTumbler.getNumberModel: value", val, " out of interval [", minimumValue, ",", maximumValue, "]")
        }
        return mo
    }

    function updateTumblerModels(from_model) {

        console.log("updateTumblerModels IN from ", from_model)
        disconnectTumbler()

        var i, j
        var digit
        var n
        // cycle through all digits
        for(i = 1; i < from_model.length; i++) {
            var modeltmp = from_model.slice()
            var ith_model = []
            if(i !== intDigits + 1) {
                // save digit
                var tumc = columns[i]
                digit = tumc.model[tumc.currentIndex]
                for(j = 0; j < 10; j++) {
                    modeltmp[i] = j
                    n = toValue(modeltmp)
                    if(n <= maximumValue && n >= minimumValue)
                        ith_model.push(j)
                    else
                        break;
                }
                if(tumc.model.length !== ith_model.length) // very basic comparison
                {
                    console.log("changing model of column ", i , "from", tumc.model, " to", ith_model)
                    tumc.model = ith_model
                    if(tumc.model[tumc.currentIndex] !== digit) {
                        if(tumc.model.indexOf(digit) > -1)
                            tumc.currentIndex = tumc.model.indexOf(digit)
                        else
                            tumc.currentIndex = tumc.model.length - 1
                    }
                }
            }
        }
        console.log("updateTumblerModels OUT")
        connectTumbler()
    }

    function tumblerColumnChanged() {
        var old_model = nmodel  // save old model
        var newmodel = getModel()
        nmodel = newmodel

        notifyIfModelChanged(old_model)
    }

    /*! \brief update the value from the model
         *
         * @param the new model
         */
    function toValue(amodel) {
        //
        // calculate value from the model instead of querying again all columns!
        //

        var val = 0.0;

        // integer part
        var i
        for(i = 1; i < intDigits + 1; i++) {
            val += amodel[i] * Math.pow(10, intDigits - i)
        }

        // decimal part
        var dexp = -1;
        for(i = intDigits + 2; i < amodel.length; i++) {
            val += amodel[i] * Math.pow(10, dexp--)
        }
        if(amodel[0] === "-")
            val = -val

        console.log("toValue from model", amodel, " we got ", val)
        return val
    }

    function adjustToBounds(amodel) {
        console.log("calling toValue from adjustToBounds")
        var val_from_model = toValue(amodel)
        if(val_from_model > maximumValue) {
            return getNumberModel(maximumValue)
        }
        else if(val_from_model < minimumValue) {
            return getNumberModel(minimumValue)
        }
        return amodel
    }

    /*! \brief returns the model represented by the tumbler value
     *   @return the model (array) representing the currently displayed value
     *
     * \par example
     * If the tumbler displays +120.03, the returned model is ["+",1,2,0,".",0,3]
     */
    function getModel() {
        var newmodel = []
        for(var n = 0; n < columns.length; n++) {
            var acol = container.columns[n]
            newmodel.push(acol.model[acol.currentIndex])
        }
        return newmodel
    }

    function notifyIfModelChanged(old_model) {
        for(var j = 0; j < nmodel.length && old_model.length === nmodel.length; j++) {
            if(nmodel[j] !== old_model[j]) {
                var from = old_model[j]
                var to = nmodel[j]
                container.columnChanged(j, from, to)
                break
            }
        }
    }

    function intDigitsFromModel() {
        return nmodel.indexOf(".") - 1
    }

    function decDigitsFromModel() {
        return nmodel.length - intDigitsFromModel() + 2
    }

    function create() {
        var currentIntDigits = intDigitsFromModel()
        var currentDecDigits = decDigitsFromModel()

        if(currentDecDigits === decDigits && currentIntDigits === intDigits) {
            console.log("create: integer and decimal digits count unchanged. No need to recreate tumbler")
            return;
        }

        var i
        for(i = 0; i < columns.length; i++) {
            columns[i].destroy()
        }
        columns = []
        if(applybutton !== null)
            applybutton.destroy()

        var number_model =  [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        var tumblerqml = "import QtQuick.Controls 2.3; Tumbler {
                 visibleItemCount:3; }"

        // plus / minus sign
        var pms = Qt.createQmlObject(tumblerqml, row_layout)
        pms.model = ["+", "-"]
        columns.push(pms)

        for(i = 0; i < intDigits; i++){
            var col = Qt.createQmlObject(tumblerqml, row_layout)
            col.model = number_model
            columns.push(col)
        }

        var commacol = Qt.createQmlObject(tumblerqml, row_layout)
        commacol.model = ["."]
        columns.push(commacol)

        for(var decd = 0; decd < decDigits; decd++){
            var decd_col = Qt.createQmlObject(tumblerqml, row_layout)
            decd_col.model = number_model
            columns.push(decd_col)
        }

        connectTumbler()
        for(var c = 0; c < columns.length; c++) {
            columns[c].Layout.fillHeight=true // fills height
            columns[c].Layout.fillWidth =true // important!
        }

        applybutton = Qt.createQmlObject("import QtQuick 2.9;
    import QtQuick.Controls 2.4; CuButton {  }", row_layout);
        applybutton.Layout.margins = 6
        applybutton.text = applyButtonText
        applybutton.onClicked.connect(apply)

    }

    function updateDigits() {
        var currentIntDigits = nmodel.indexOf(".") - 1
        console.log("updateDigits, current", currentIntDigits, " will be", intDigits)
        if(currentIntDigits > intDigits) {
            for(var i = 1; i < currentIntDigits - intDigits + 1; i++) {
                var tcol = columns[i]
                console.log("updateDigits: remogin column at ", i, tcol)
                tcol.destroy();
            }

        }
    }
}
