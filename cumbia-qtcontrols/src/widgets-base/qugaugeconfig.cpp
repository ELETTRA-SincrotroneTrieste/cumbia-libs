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
    tickLen = 0.08; // percentage of radius
    gaugeWidth = 6;

    format = "%.0f";

    drawBackground = false;
    drawTickCore1 = true;
    drawTickCore2 = false;

    warningColor = QColor(Qt::yellow);
    errorColor = QColor(Qt::red);
    normalColor = QColor(Qt::green);
    backgroundColor = QColor(Qt::white);
    backgroundGradientSpread = 0.9;
    backgroundColorAlpha = 100;
    needleColor = QColor(Qt::red);
    tickCoreColor = QColor(Qt::gray);
    textColor = QColor(Qt::black);
}
