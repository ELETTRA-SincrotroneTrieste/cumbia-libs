import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0



CumbiaSpectrumChart {
    sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/float_spectrum_ro;tango://ken.elettra.trieste.it:20000/test/device/1/double_spectrum_ro"
    id: spectrumChart
    title: "float, double spectrum"
    //        backend.period: 10
}

