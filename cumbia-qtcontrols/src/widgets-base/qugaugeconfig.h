#ifndef QUGAUGECONFIG_H
#define QUGAUGECONFIG_H

#include <QStringList>
#include <QColor>

class QuGaugeConfig
{
public:
    QuGaugeConfig();

    double value, min, max, low_w, high_w, low_e, high_e;
    double value_anim; // value used for animation
    int ticksCount;
    double tickLen;
    double backgroundGradientSpread, gaugeWidth;
    int backgroundColorAlpha;
    bool drawBackground, drawTickCore1, drawTickCore2;
    bool animationEnabled;
    bool drawText;
    qint64 maximumAnimDuration;

    QString format;

    QStringList labelsCache;
    QColor errorColor, warningColor, normalColor, backgroundColor;
    QColor needleColor, tickCoreColor;
    QColor textColor;
};

#endif // QUGAUGECONFIG_H
