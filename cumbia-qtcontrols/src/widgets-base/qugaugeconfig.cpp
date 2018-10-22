#include "qugaugeconfig.h"

QuGaugeConfig::QuGaugeConfig()
{
    minFontSize = 8;
    min = -1000;
    max = 1000;
    value = 0;
    value_anim = 0;
    animationEnabled = true;
    drawText = true;
    maximumAnimDuration = 1800; // millis
    unit = "";

    // if low_w == high_w low warning is disabled
    // the same goes for low/high error
    //
    low_w = high_w = low_e = high_e = 0.0;

    ticksCount = 11;
    minorTicksCount = 5;
    tickLen = 0.08; // percentage of radius respect to min(width, height) (0.08 = 8% )
    arrowSize = 1.0; // percentage of tickLen (1.0 = 100%)
    gaugeWidth = 6;
    pivotRadius = 0.08;

    format = "%.0f";

    // drawing options
    drawBackground = true;
    drawTickCore1 = true;
    drawTickCore2 = true;
    drawColoredTicks = false;

    warningColor = QColor("orange"); // http://www.december.com/html/spec/colorsvg.html
    errorColor = QColor(Qt::red);
    normalColor = QColor(Qt::green);
    readErrorColor = QColor(Qt::darkGray);
    backgroundColor = QColor(Qt::white);
    backgroundGradientSpread = 0.9;
    backgroundColorAlpha = 100;
    tickCoreColor = QColor(Qt::black);
    textColor = QColor(Qt::black);

    needleColor = QColor(Qt::red);
    arrowColor = needleGuideColor = QColor(Qt::white);
    arrowColor.setAlphaF(0.8);
    arrowPenWidth = 1.2f;
    thermo_width = 0.2; // percentage of widget height

    // Scalable Vector Graphics Color Names
    // http://www.december.com/html/spec/colorsvg.html
    thermoColor1 = QColor("dodgerblue");
    thermoColor2 = QColor("deepskyblue");
    thermoPenColor = QColor(Qt::white);
}
