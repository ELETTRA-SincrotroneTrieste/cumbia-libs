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
    float minFontSize;
    bool drawBackground, drawTickCore1, drawTickCore2, drawColoredTicks;
    bool animationEnabled;
    bool drawText;
    qint64 maximumAnimDuration;

    QString format;
    QString unit;

    QStringList labelsCache;
    QColor errorColor, warningColor, normalColor, backgroundColor, readErrorColor;
    QColor needleColor, tickCoreColor;
    QColor needleGuideColor; // linear gauge, needle indicator only
    QColor textColor;

    QColor arrowColor; // linear gauge only
    double arrowSize; // linear gauge only
    double arrowPenWidth; // linear gauge only
    double thermo_width; // linear gauge, thermo indicator only
    QColor thermoColor1, thermoColor2, thermoPenColor;
};

#endif // QUGAUGECONFIG_H
