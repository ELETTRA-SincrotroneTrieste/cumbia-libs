#ifndef QUGAUGECONFIG_H
#define QUGAUGECONFIG_H

#include <QStringList>
#include <QColor>
#include <QPen>

class QuGaugeConfig
{
public:
    QuGaugeConfig();

    double value, min, max, low_w, high_w, low_e, high_e;
    double value_anim; // value used for animation
    int ticksCount, minorTicksCount;
    double tickLen;
    double backgroundGradientSpread, gaugeWidth;
    double pivotRadius;
    int backgroundColorAlpha;
    bool drawBackground, drawTickCore1, drawTickCore2, drawColoredTicks;
    bool animationEnabled;
    bool drawText;
    qint64 maximumAnimDuration;

    QString format;
    QString unit;

    QStringList labelsCache;
    QColor errorColor, warningColor, normalColor, backgroundColor;
    QColor needleColor, tickCoreColor;
    QColor textColor;

    QColor arrowColor; // linear gauge only
    double arrowSize; // linear gauge only
    double arrowPenWidth; // linear gauge only
};

#endif // QUGAUGECONFIG_H
