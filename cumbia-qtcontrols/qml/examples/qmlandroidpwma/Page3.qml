import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0



CumbiaTrendChart {
    sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar;tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar;tango://ken.elettra.trieste.it:20000/test/device/1/short_scalar"
    id: scalarChart
    title: "double, long, short"
    //        backend.period: 10
}

