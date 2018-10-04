#include "qugaugeconfig.h"

QuGaugeConfig::QuGaugeConfig()
{
    min = -1000;
    max = 1000;
    value = 0;
    value_anim = 0;
    animationEnabled = true;
    drawText = true;
    maximumAnimDuration = 1800; // millis

    // if low_w == high_w low warning is disabled
    // the same goes for low/high error
    //
    low_w = high_w = low_e = high_e = 0.0;

    ticksCount = 10;
    minorTicksCount = 5;
    tickLen = 0.08; // percentage of radius respect to min(width, height) (0.08 = 8% )
    arrowSize = 1.0; // percentage of tickLen (1.0 = 100%)
    gaugeWidth = 6;

    format = "%.0f";

    // drawing options
    drawBackground = true;
    drawTickCore1 = true;
    drawTickCore2 = true;
    drawColoredTicks = false;

    warningColor = QColor(Qt::yellow);
    errorColor = QColor(Qt::red);
    normalColor = QColor(Qt::green);
    backgroundColor = QColor(Qt::white);
    backgroundGradientSpread = 0.9;
    backgroundColorAlpha = 100;
    tickCoreColor = QColor(Qt::black);
    textColor = QColor(Qt::black);

    needleColor = QColor(Qt::black);
    arrowColor = QColor(Qt::white);
    arrowColor.setAlphaF(0.8);
    arrowPenWidth = 1.2f;
}
