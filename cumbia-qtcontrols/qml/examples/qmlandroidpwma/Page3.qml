import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0



CumbiaTrendChart {
    sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
    id: scalarChart
    title: "float_spectrum_ro"
    //        backend.period: 10
}

