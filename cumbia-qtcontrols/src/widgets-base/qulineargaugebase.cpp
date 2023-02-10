#include "qulineargaugebase.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QtDebug>
#include <cumacros.h>
#include <math.h>

class QuLinearGaugeCache {
public:
    QString longestLabel;
    QStringList labels;
    QRectF paintArea;
    QRectF oldRect;
    QSize labelSize;
    QRectF labelRect;
    float labelPointSize;

    void invalidate(){
        longestLabel = QString();
        paintArea = QRectF();
        labelRect = QRectF();
        oldRect = QRectF();
        labelPointSize = 10;
        labelSize = QSize();
    }

    bool isValid() const {
        return longestLabel.length() > 0;
    }
};

class QuLinearGaugeBasePrivate {
public:
    QString label;   // text property. If empty, value is displayed
    QString label_p; // text currently displayed (label property or current value)
    QString labelValueFormat; // if label displays value, use this format
    QSize minSizeHint, sizeHint;
    QuLinearGaugeCache cache;
    QPropertyAnimation *propertyAnimation;
    QDateTime lastValueDateTime;
    QuLinearGaugeBase::LabelPosition labelPosition;
    QuLinearGaugeBase::IndicatorType indicatorType;
    QuLinearGaugeBase::ScaleMode scaleMode;
    double labelFontScale;
    bool readError;
};

QuLinearGaugeBase::QuLinearGaugeBase(QWidget *parent) : QWidget(parent)
{
    g_config = new QuGaugeConfig;
    g_config->gaugeWidth = 0.50f;
    g_config->ticksCount = 11;
    g_config->tickLen = 0.2;
    g_config->pivotRadius = 0.2;
    d = new QuLinearGaugeBasePrivate;
    d->propertyAnimation = NULL;
    d->labelPosition = NoLabel;
    d->labelValueFormat = "%.2f";
    d->labelFontScale = 0.9;
    d->scaleMode = Normal;
    d->indicatorType = Needle;
    d->readError = false;
}

QuLinearGaugeBase::~QuLinearGaugeBase()
{
    if(d->propertyAnimation) {
        d->propertyAnimation->stop();
        delete d->propertyAnimation;
    }
    delete d;
    delete g_config;
}

double QuLinearGaugeBase::value() const
{
    return g_config->value;
}

bool QuLinearGaugeBase::readError() const
{
    return d->readError;
}

/*! \brief returns the value during the animation
 *
 * To get the actual value, please refer to the QuLinearGaugeBase::value method
 */
double QuLinearGaugeBase::value_anim() const
{
    return g_config->value_anim;
}

/*! \brief returns the maximum value represented by this gauge
 *
 * @return the maximum value of this gauge's scale
 */
double QuLinearGaugeBase::maxValue() const
{
    return g_config->max;
}

/*! \brief returns the minimum value represented by this gauge
 *
 * @return the minimum value of this gauge's scale
 */
double QuLinearGaugeBase::minValue() const
{
    return g_config->min;
}

/*! \brief returns the number of *major* ticks drawn in the arc
 *
 * @return the number of *major ticks* drawn
 *
 * @see minorTicksCount
 */
int QuLinearGaugeBase::ticksCount() const
{
    return g_config->ticksCount;
}

/*! \brief returns the number of *minor* ticks drawn between each couple of major ticks
 * @return the number of minor ticks between two subsequent major ticks
 *
 * @see ticksCount
 * @see setMinorTicksCount
 */
int QuLinearGaugeBase::minorTicksCount() const
{
    return g_config->minorTicksCount;
}

/*! \brief returns the tick length
 *
 * The value, expressed within the interval [0.0, 1.0] is intended as fraction
 * of the lenght of the gauge arc's radius.
 *
 * \par Default value
 * The default value is 0.08. The major tick length is thus equal to 2% of the radius by default.
 */
double QuLinearGaugeBase::tickLen() const
{
    return g_config->tickLen;
}

/*! \brief returns the *width* of the gauge strip
 *
 * @return the width, in pixels, floating point, used to draw the line
 *
 */
double QuLinearGaugeBase::gaugeWidth() const
{
    return g_config->gaugeWidth;
}

/*! \brief returns the arrow size, percentage
 *
 * arrowSize property expresses the size of the arrow respect to the tickLen
 * An arrowSize of 1.0 will produce an arrow with the side equal to the (major) tickLen
 *
 * @see setArrowSize
 */
double QuLinearGaugeBase::arrowSize() const
{
    return g_config->arrowSize;
}

/*! \brief returns the width of the thermometer bar, when indicatorMode is set to Thermo
 *
 * The width of the thermometer is expressed as fraction of the widget's smallest size.
 * The default value of 0.3 will draw a bar of 30 pixels on a gauge with a size 200x90
 *
 * \note This property applies only to linear gauges with the indicatorMode property set to
 *       Thermo
 *
 * @see setThermoWidth
 * @see setIndicatorMode
 */
double QuLinearGaugeBase::thermoWidth() const
{
    return g_config->thermo_width;
}

/*! \brief the factor multiplies tickLen to obtain the radius for the circle
 *         drawn at the base of the needle.
 *
 * @return a factor determining the radius of the circle drawn as pivot of the
 *         needle.
 *
 * @see setPivotCircleRadius
 * @see
 */
double QuLinearGaugeBase::pivotCircleRadius() const
{
    return g_config->pivotRadius;
}

/*! \brief returns true if a background is drawn, false otherwise
 *
 * @return true the gauge draws a background with a radial gradient between the
 *         color associated the current value (see inErrorRange, inWarningRange,
 *         inNormalRange) and the color chosen with setBackgroundColor.
 *
 * Three warning levels can be displayed by the gauge arc:
 * \li normal
 * \li warning
 * \li alarm (error)
 *
 * Setting the drawBackgroundEnabled property to true enhances the detection of the current
 * value quality.
 *
 * @see backgroundColor
 *
 */
bool QuLinearGaugeBase::drawBackgroundEnabled() const {
    return g_config->drawBackground;
}

/*! \brief either draw or not the labels with the values associated to each major tick
 *
 * @return true the value of each major tick is drawn
 *         false no values are drawn on the gauge
 *
 * \note if you want the current value to be displayed on a separate bigger label somewhere
 *       in the widget area, use the labelPosition property and set it to a value different
 *       from QuLinearBase::NoLabel. At the same time, the label property must be set to
 *       an empty string.
 *
 * @see label
 * @see setLabelPosition
 * @see setLabel
 */
bool QuLinearGaugeBase::drawText() const {
    return g_config->drawText;
}

/*! \brief draw a tick inside the gauge arc
 *
 * @return true a tick is drawn within the gauge arc. Its width is equal to the gaugeWidth
 *          property (in pixels)
 *
 * @see drawTickCore2
 * @see drawColoredTicks
 */
bool QuLinearGaugeBase::drawTickCore1() const {
    return g_config->drawTickCore1;
}

/*! \brief draw a tick from the inner border of the gauge arc
 *
 * @return true a tick is drawn from the inner border of the gauge arc. Its width is equal to the gaugeWidth
 *          property (in pixels)
 *
 * @see drawTickCore1
 * @see drawColoredTicks
 */
bool QuLinearGaugeBase::drawTickCore2() const
{
    return g_config->drawTickCore2;
}

/*! \brief draw colored ticks
 *
 * This property can also be enabled in association with drawTickCore1 or drawTickCore2 if the effect
 * is welcome.
 *
 * If enabled, the colored ticks length is equal to *radius multiplied by (1.0 - tickLen)*.
 */
bool QuLinearGaugeBase::drawColoredTicks() const
{
    return g_config->drawColoredTicks;
}

/*! \brief if true, an animation is used to draw the transition between two subsequent setValue calls
 *
 * @return true the needle is animated when moving from its current position to the new position
 *         specified with setValue
 *         false the needle abruptly moves to the new value
 *
 * \par Special note
 * The maximum duration of the animation is determined by the value, in milliseconds, of the maxAnimationDuration property.
 * If two setValue calls take place within a shorter interval, that interval is used to animate the transition instead.
 *
 * @see maxAnimationDuration
 *
 */
bool QuLinearGaugeBase::animationEnabled() const
{
    return g_config->animationEnabled;
}

/*! \brief The maximum duration of an animation between two setValue calls
 *
 * @return the value, in milliseconds, of the  maximum duration of an animation between two setValue calls
 *
 * \par note
 * If two setValue calls take place within a shorter interval, that interval is used to animate the transition instead.
 *
 * @see animationEnabled
 *
 */
qint64 QuLinearGaugeBase::maxAnimationDuration() const
{
    return g_config->maximumAnimDuration;
}

QuLinearGaugeBase::LabelPosition QuLinearGaugeBase::labelPosition() const
{
    return d->labelPosition;
}

QuLinearGaugeBase::IndicatorType QuLinearGaugeBase::indicatorType() const
{
    return d->indicatorType;
}

QuLinearGaugeBase::ScaleMode QuLinearGaugeBase::scaleMode() const
{
    return d->scaleMode;
}

/*! \brief scales the label font respect to the font size of the text on the gauge
 *
 * @return the label font scale value, between 0 and 2.
 *
 * The font size of the text on the gauge is scaled by this factor.
 *
 * \note The default value is 0.9
 */
double QuLinearGaugeBase::labelFontScale() const
{
    return d->labelFontScale;
}

void QuLinearGaugeBase::setLowWarning(double w) {
    g_config->low_w = w;
    update();
}

double QuLinearGaugeBase::lowWarning() {
    return g_config->low_w;
}

void QuLinearGaugeBase::setHighWarning(double w) {
    g_config->high_w = w;
    update();
}

double QuLinearGaugeBase::highWarning() {
    return g_config->high_w;
}

double QuLinearGaugeBase::lowError(){
    return g_config->low_e;
}

QColor QuLinearGaugeBase::textColor() const
{
    return g_config->textColor;
}

QColor QuLinearGaugeBase::warningColor() const
{
    return g_config->warningColor;
}

QColor QuLinearGaugeBase::errorColor() const
{
    return g_config->errorColor;
}

QColor QuLinearGaugeBase::normalColor() const
{
    return g_config->normalColor;
}

QColor QuLinearGaugeBase::readErrorColor() const
{
    return g_config->readErrorColor;
}

QColor QuLinearGaugeBase::backgroundColor() const
{
    return g_config->backgroundColor;
}

QColor QuLinearGaugeBase::needleColor() const
{
    return g_config->needleColor;
}

/*! \brief returns the color of the needle guide, that is drawn along the longest
 *         widget side and crosses the needle pivot center
 *
 * @return the color of the <cite>needle guide</cite>
 */
QColor QuLinearGaugeBase::needleGuideColor() const
{
    return g_config->needleGuideColor;
}

QColor QuLinearGaugeBase::tickCoreColor() const
{
    return g_config->tickCoreColor;
}

/*! \brief returns the arrow fill color.
 *
 * The arrow is filled with the arrowColor color. Its outline is drawn with the
 * needleColor
 *
 * \note
 * The default colour is white for the arrow fill with an alpha of 150
 *
 * @see needleColor
 * @see arrowSize
 */
QColor QuLinearGaugeBase::arrowColor() const
{
    return g_config->arrowColor;
}

/*! \brief Sets the color 1 (inner color) used to draw the gradient for the thermo bar
 *
 * If the gauge indicator is Thermo, a thermometer like bar is drawn using a linear
 * gradient. This property holds the color 1 of the gradient, used to fill the inner
 * section of the bar.
 *
 * @see thermoColor2
 * @see setThermoColor1
 * @see setIndicatorMode
 */
QColor QuLinearGaugeBase::thermoColor1() const
{
    return g_config->thermoColor1;
}

/*! \brief Sets the color 2 (outer color) used to draw the gradient for the thermo bar
 *
 * If the gauge indicator is Thermo, a thermometer like bar is drawn using a linear
 * gradient. This property holds the color 2 of the gradient, used to colr the outer
 * sides of the bar.
 *
 * @see thermoColor1
 * @see setThermoColor2
 * @see setIndicatorMode
 */
QColor QuLinearGaugeBase::thermoColor2() const
{
    return g_config->thermoColor2;
}

/*! \brief returns the pen color used to outline the thermometer bar in the gauge indicator
 *
 * @return the pen color
 * \note applies to QuLinearGauge elements which indicatorProperty is set
 *       to QuLinearGaugeBase::Thermo
 *
 * @see thermoColor1
 * @see thermoColor2
 * @see setThermoPenColor
 * @see indicatorMode
 */
QColor QuLinearGaugeBase::thermoPenColor() const
{
    return g_config->thermoPenColor;
}

double QuLinearGaugeBase::arrowPenWidth() const
{
    return g_config->arrowPenWidth;
}

double QuLinearGaugeBase::backgroundGradientSpread() const
{
    return g_config->backgroundGradientSpread;
}

int QuLinearGaugeBase::backgroundColorAlpha() const
{
    return g_config->backgroundColorAlpha;
}

Qt::Orientation QuLinearGaugeBase::orientation() const
{
    if(width() >= height())
        return Qt::Horizontal;
    return Qt::Vertical;
}

void QuLinearGaugeBase::setLowError(double e) {
    g_config->low_e = e;
    update();
}

void QuLinearGaugeBase::setTextColor(const QColor &c)
{
    g_config->textColor = c;
    update();
}

void QuLinearGaugeBase::setErrorColor(const QColor &ec)
{
    g_config->errorColor = ec;
    update();
}

void QuLinearGaugeBase::setWarningColor(const QColor &wc)
{
    g_config->warningColor = wc;
    update();
}

void QuLinearGaugeBase::setNormalColor(const QColor &nc)
{
    g_config->normalColor = nc;
    update();
}

void QuLinearGaugeBase::setReadErrorColor(const QColor &rec)
{
    g_config->readErrorColor = rec;
    update();
}

void QuLinearGaugeBase::setBackgroundColor(const QColor &bc)
{
    g_config->backgroundColor = bc;
    update();
}

void QuLinearGaugeBase::setNeedleColor(const QColor &c)
{
    g_config->needleColor = c;
    update();
}

void QuLinearGaugeBase::setNeedleGuideColor(const QColor &c)
{
    g_config->needleGuideColor = c;
    update();
}

void QuLinearGaugeBase::setPivotCircleRadius(double percent)
{
    g_config->pivotRadius = percent;
    update();
}

/*! \brief sets the arrow fill color
 *
 * @see arrowColor
 */
void QuLinearGaugeBase::setArrowColor(const QColor &c)
{
    g_config->arrowColor  = c;
    update();
}

/*! \brief changes the color 1 of the gradient used to draw the bar of a Thermo indicator
 *
 * @see thermoColor1
 */
void QuLinearGaugeBase::setThermoColor1(const QColor &c)
{
    g_config->thermoColor1 = c;
    update();
}

/*! \brief changes the color 2 of the gradient used to draw the bar of a Thermo indicator
 *
 * @see thermoColor2
 */
void QuLinearGaugeBase::setThermoColor2(const QColor &c)
{
    g_config->thermoColor2 = c;
    update();
}

/*! \brief changes the color used to outline the thermometer bar
 *
 * \note applies to QuLineargauge with the property indicatorMode equal to QuLinearGaugeBase::Thermo
 */
void QuLinearGaugeBase::setThermoPenColor(const QColor &c)
{
    g_config->thermoPenColor = c;
    update();
}

void QuLinearGaugeBase::setArrowPenWidth(double pw)
{
    g_config->arrowPenWidth = pw;
    update();
}

void QuLinearGaugeBase::setTickCoreColor(const QColor &c)
{
    g_config->tickCoreColor = c;
    update();
}

/** \brief If background drawing is enabled, the spread determines the stop point
 *         of the gradient from the current value color (normal, warning or error)
 *         and the background color
 *
 * @param s the gradient stop, greater than 0 and less than or equal to 1
 *
 * @see setBackgroundColor
 * @see drawBackgroundEnabled
 * @see backgroundColor
 */
void QuLinearGaugeBase::setBackgroundGradientSpread(double s)
{
    if(s > 0.0 && s <= 1) {
        g_config->backgroundGradientSpread = s;
        update();
    }
    else
        perr("QuLinearGaugeBase::setBackgroundGradientSpread: value must be 0 < value <= 1");
}

void QuLinearGaugeBase::setBackgroundColorAlpha(int a)
{
    g_config->backgroundColorAlpha = a;
    update();
}

void QuLinearGaugeBase::setDrawTickCore1(bool t)
{
    g_config->drawTickCore1 = t;
    update();
}

void QuLinearGaugeBase::setDrawTickCore2(bool t)
{
    g_config->drawTickCore2 = t;
    update();
}

void QuLinearGaugeBase::setDrawColoredTicks(bool dr)
{
    g_config->drawColoredTicks = dr;
    update();
}

void QuLinearGaugeBase::setDrawText(bool dt)
{
    g_config->drawText = dt;
    update();
}

void QuLinearGaugeBase::setAnimationEnabled(bool ae)
{
    g_config->animationEnabled = ae;
    update();
}

void QuLinearGaugeBase::setMaxAnimationDuration(qint64 millis)
{
    if(millis > 0)
        g_config->maximumAnimDuration = millis;
}

void QuLinearGaugeBase::setLabelPosition(QuLinearGaugeBase::LabelPosition p)
{
    d->labelPosition = p;
    d->cache.invalidate();
    update();
    updateGeometry();
}

void QuLinearGaugeBase::setIndicatorType(QuLinearGaugeBase::IndicatorType t)
{
    d->indicatorType = t;
    update();
}

void QuLinearGaugeBase::setScaleMode(QuLinearGaugeBase::ScaleMode sm)
{
    d->scaleMode = sm;
    update();
}

void QuLinearGaugeBase::setLabelValueFormat(const QString &f)
{
    d->labelValueFormat = f;
    update();
}

void QuLinearGaugeBase::setUnit(const QString &u)
{
    g_config->unit = u;
    update();
}

/*! \brief changes the label font size respect to the size of the text on the gauge
 *
 * @param factor the label font scale value, > 0 and  <= 2.
 *
 * The font size of the text on the gauge is scaled by this factor.
 *
 * \note The default value is 0.9
 */
void QuLinearGaugeBase::setLabelFontScale(double factor)
{
    if(factor > 0 && factor <= 2.0) {
        d->labelFontScale = factor;
        updateGeometry();
    }
    else
        perr("QuLinearGaugeBase::setLabelFontScale: factor must be  0 < factor <= 2.0");
}

void QuLinearGaugeBase::setHighError(double e) {
    g_config->high_e = e;
    update();
}

double QuLinearGaugeBase::highError() {
    return g_config->high_e;
}

QString QuLinearGaugeBase::label() const
{
    return d->label;
}

QString QuLinearGaugeBase::format() const
{
    return g_config->format;
}

bool QuLinearGaugeBase::formatFromPropertyEnabled() const
{
    return g_config->format_from_property;
}

QString QuLinearGaugeBase::getAppliedFormat() const
{
    if(g_config->format_from_property)
        return g_config->format_prop;
    return g_config->format;
}

QString QuLinearGaugeBase::labelValueFormat() const
{
    return d->labelValueFormat;
}

QString QuLinearGaugeBase::unit() const
{
    return g_config->unit;
}

/*! \brief maps a value to a point along the longest side of the widget rectangle
 *
 * @return a QPointF close to the arc of the gauge. How close it is to the arc is determined by tickLen:
 *         the point will be distant 1.2 * tickLen from the arc
 *
 * @param val the value set on the widget
 * @param angle a pointer to a double that will store the angle, in degrees, from the 3 o'clock axis to the
 *        needle (if angle is not NULL)
 * @param radius if greater than 0, the parameter is used to return a point with such distance from the
 *        center. Otherwise, radius is assumed to be equal to the paint area width divided by two.
 * @param paint_area if valid, it is used for the calculation. Otherwise the paintArea method is used to
 *        recalculate the paint area
 */
double QuLinearGaugeBase::mapTo(double val, const QRectF& paint_area)
{
    double v = 0.0, len, offs;
    QRectF paintA;
    !paint_area.isValid() ? paintA = paintArea() : paintA = paint_area;
    paintA.width() > paintA.height() ? len = paintA.width() : len = paintA.height();
    paintA.width() > paintA.height() ? offs = paintA.left() : offs = paintA.top();
    if(g_config->max >= g_config->min && val <= g_config->max && val >= g_config->min) {
        v = offs + len * (val - g_config->min) / (g_config->max - g_config->min);
    }
    return v;
}


///
/// FARE thermo indicator
/// controllare minimum size (+ hint)
///

QRectF QuLinearGaugeBase::textRect(const QRectF &paintArea, int tick)
{
    int idx = 0;
    int step = qRound((g_config->max - g_config->min) / (g_config->ticksCount - 1));
    double mindim, offs;
    mindim = paintArea.height();
    double tickLen = mindim * g_config->tickLen;
    double val = g_config->min;
    double y0;
    d->scaleMode == Normal ? y0 = paintArea.top() + tickLen * 1.2 :
            y0 = paintArea.top() + getGaugeWidth(paintArea) - tickLen * 1.2 - d->cache.labelSize.height();
    while(val <= g_config->max) {
        if(idx == tick) {
            double labelPos = mapTo(val, paintArea);
            QRectF trect(labelPos, y0, d->cache.labelSize.width(), d->cache.labelSize.height());
            if(tick == 0) offs = -paintArea.left();
            else if(tick == g_config->ticksCount - 1) offs =   - paintArea.left() -trect.width()/2.0;
            else offs = -trect.width()/2.0;
            trect.moveLeft(trect.left() +offs);
            return trect;
        }
        val += step;
        idx++;
    }
    return QRectF();
}

double QuLinearGaugeBase::labelFontSize()
{
    if(!d->cache.isValid()) {
        regenerateCache();
    }
    return d->cache.labelPointSize;
}

QString QuLinearGaugeBase::formatLabel(double value, const char* format) const
{
    char clab[32];
    snprintf(clab, 32, format, value);
    return QString(clab);
}

QString QuLinearGaugeBase::label(int i) const
{
    if(i < d->cache.labels.size())
        return d->cache.labels[i];
    return QString();
}

void QuLinearGaugeBase::setValue(double v)
{
    QDateTime now = QDateTime::currentDateTime();
    if(g_config->animationEnabled) {
        if(d->propertyAnimation && d->propertyAnimation->state() == QPropertyAnimation::Running) {
            d->propertyAnimation->stop();
        }
        else if(!d->propertyAnimation) {
            d->propertyAnimation = new QPropertyAnimation(this, "value_anim");
        }

        if(d->lastValueDateTime.isValid())
            d->propertyAnimation->setDuration(qMin(g_config->maximumAnimDuration, d->lastValueDateTime.msecsTo(now)));
        else
            d->propertyAnimation->setDuration(g_config->maximumAnimDuration);

        v > g_config->value_anim ? d->propertyAnimation->setEasingCurve(QEasingCurve(QEasingCurve::Linear)) :
                                   d->propertyAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InQuad));

        d->propertyAnimation->setStartValue(g_config->value_anim);
        d->propertyAnimation->setEndValue(v);
        d->propertyAnimation->start();
    }
    else {
        g_config->value_anim = v;
        update();
    }

    g_config->value = v;
    d->lastValueDateTime = now;

}

void QuLinearGaugeBase::setReadError(bool re)
{
    d->readError = re;
    update();
}

void QuLinearGaugeBase::setValue_anim(double v)
{
    g_config->value_anim = v;
    update();
}

void QuLinearGaugeBase::setMinValue(double min)
{
    if(min < g_config->max) {
        g_config->min = min;
        regenerateCache();
        update();
    }
}

void QuLinearGaugeBase::setMaxValue(double max)
{
    if(max > g_config->min ) {
        g_config->max = max;
        regenerateCache();
        update();
    }
}

void QuLinearGaugeBase::setLabel(const QString &l)
{
    d->label = l;
    regenerateCache();
    update();
}

void QuLinearGaugeBase::setTicksCount(int t)
{
    g_config->ticksCount = t;
    regenerateCache();
    update();
}

void QuLinearGaugeBase::setMinorTicksCount(int t)
{
    g_config->minorTicksCount = t;
    update();
}

/** \brief Set the tick len as percentage of the radius of the circle
 *
 * @param tl a value between 0 and 1 representing the length of a tick with respect
 *        to the length of the radius
 */
void QuLinearGaugeBase::setTickLen(double tl)
{
    g_config->tickLen = tl;
    regenerateCache();
    update();
}

void QuLinearGaugeBase::setGaugeWidth(double w)
{
    g_config->gaugeWidth = w;
    update();
}

/*! \brief sets the arrow size property
 *
 * The arrow size will be propotional to the major tick length (tickLen property)
 * A value of 1.0 will produce an arrow which side is equal to the tick length
 *
 * @see arrowSize
 */
void QuLinearGaugeBase::setArrowSize(double s)
{
    g_config->arrowSize = s;
    update();
}

/*! \brief sets the thermoWidth property
 *
 * @param tw the new thermoWidth value, fraction of the widget's smallest side
 *
 * \note This applies to linear gauges with indicatorMode property equal to Thermo
 *
 * @see thermoWidth
 */
void QuLinearGaugeBase::setThermoWidth(double tw)
{
    g_config->thermo_width = tw;
    update();
}

void QuLinearGaugeBase::setDrawBackgroundEnabled(bool en)
{
    g_config->drawBackground = en;
}

/*! \brief sets the format to be used on the text along the gauge if the property
 *         formatFromProperty enabled is false
 *
 * @param f the format used if formatFromProperty is false
 *
 * If formatFromProperty is true, the format is applied according to the value of the "format" key
 * in the *configuration* CuData for the given source.
 *
 * \par Note
 * The *configuration* CuData is the one where the *type* key has the *property* value
 *
 * @see setFormatFromPropertyEnabled
 */
void QuLinearGaugeBase::setFormat(const QString &f)
{
    g_config->format = f;
    regenerateCache();
    update();
}

/*! \brief enables or disables applying format from the *configuration* CuData
 *
 * @param ffp true the format of the text along the gauge is taken by the source properties.
 * @param ffp false the format of the text along the gauge is taken by the format property
 *
 * @see format
 * @see formatFromPropertyEnabled
 */
void QuLinearGaugeBase::setFormatFromPropertyEnabled(bool ffp)
{
    g_config->format_from_property = ffp;
    regenerateCache();
    update();
}

/*! \brief sets the format of the text along the gauge to be used when formatFromPropertyEnabled is true
 *
 * @param fp the format to be applied to the numbers along the gauge when formatFromPropertyEnabled is true
 *
 * \par note
 * This method is called by QuLinearGauge when a CuData has the value "property" associated to the key "type"
 *
 * @see setFormatFromPropertyEnabled
 */
void QuLinearGaugeBase::setFormatProperty(const QString &fp)
{
    g_config->format_prop = fp;
    regenerateCache();
    update();
}

void QuLinearGaugeBase::updateLabelsCache()
{
    d->cache.labels.clear();
    int step = qRound((g_config->max - g_config->min) / (g_config->ticksCount - 1));
    double val = g_config->min;
    while(val <= g_config->max) {
        QString lab = formatLabel(val, g_config->format.toStdString().c_str());
        if(lab.length() > d->cache.longestLabel.length())
            d->cache.longestLabel = lab;
        // fill labels cache
        d->cache.labels << lab;
        val += step;
    }
    updateGeometry();
}

void QuLinearGaugeBase::regenerateCache()
{
    d->cache.invalidate();
    updatePaintArea();
    updateLabelsCache();
    updateLabelsFontSize();
}

void QuLinearGaugeBase::updateLabelsFontSize() {
    QFont f = font();
    QFontMetrics fm(f);
    QRectF p_a = paintArea();
    double maxh;
    if(d->scaleMode == Normal)
        maxh = p_a.height()/2.0;
    else
        maxh = gaugeRect(p_a).height() - getTickLen(p_a);
    const double rw = ( qMax(p_a.width(), p_a.height()) - fm.horizontalAdvance(d->cache.longestLabel)) / (g_config->ticksCount + 2);
    while(fm.horizontalAdvance(d->cache.longestLabel) < rw && fm.height() < maxh) {
        f.setPointSizeF(f.pointSizeF() + 0.5);
        fm = QFontMetrics(f);
    }
    while((fm.horizontalAdvance(d->cache.longestLabel) > rw || fm.height() > maxh) && f.pointSizeF() > 4) {
        f.setPointSizeF(f.pointSizeF() - 0.5);
        fm = QFontMetrics(f);
    }
    d->cache.labelPointSize = f.pointSizeF();
    fm = QFontMetrics(f);
    d->cache.labelSize = QSize(fm.horizontalAdvance(d->cache.longestLabel), fm.height());
}

QRectF QuLinearGaugeBase::paintArea() {
    if(!d->cache.paintArea.isValid())
        updatePaintArea();
    return d->cache.paintArea;
}

double QuLinearGaugeBase::m_getMarginW(double radius)
{
    Q_UNUSED(radius)
    QFont fo = font();
    fo.setPointSizeF(d->cache.labelPointSize);
    QFontMetrics fme(fo);
    double margin_w = fme.horizontalAdvance(d->cache.longestLabel) / 2;

    return margin_w;
}

double QuLinearGaugeBase::m_getMarginH(double radius) {
    Q_UNUSED(radius)
    QFont fo = font();
    fo.setPointSizeF(d->cache.labelPointSize);
    QFontMetrics fme(fo);
    double margin_h = static_cast<double>(fme.height());

    return margin_h;
}

bool QuLinearGaugeBase::inWarningRange(double val) const
{
    if(g_config->low_w == g_config->high_w)
        return false;

    if(g_config->low_e != g_config->high_e) {
        return (val <= g_config->low_w && val > g_config->low_e ) ||
                (val >= g_config->high_w && val < g_config->high_e);
    }
    else {
        return val <= g_config->low_w  || val >= g_config->high_w;
    }

    return false;
}

bool QuLinearGaugeBase::inErrorRange(double val) const
{
    if(g_config->low_e == g_config->high_e)
        return false;
    return val <= g_config->low_e || val >= g_config->high_e;
}

QColor QuLinearGaugeBase::valueColor(double val) const
{
    QColor c;
    if(inWarningRange(val)) return g_config->warningColor;
    else if(inErrorRange(val)) return g_config->errorColor;
    else return g_config->normalColor;
}

/*! \brief returns the gauge width in pixels
 *
 * @return the gauge width, in pixels, calculated with the expression
 *         <cite>gaugeWidth * (paintArea height)</cite>
 * @param paint_area const reference to the current paint area
 * @see paintArea
 */
double QuLinearGaugeBase::getGaugeWidth(const QRectF &paint_area) const
{
    return g_config->gaugeWidth * paint_area.height();
}

/*! \brief returns the thermometer bar width in pixels
 *
 * @return the thermometer bar width in pixels or 0 if the indicatorType is not Thermo
 *
 * @param paint_area const reference to the current paint area
 * @see paintArea
 */
double QuLinearGaugeBase::getThermoWidth(const QRectF &paint_area) const
{
    if(d->indicatorType == Thermo)
        return g_config->thermo_width * paint_area.height();
    return 0;
}

/*! \brief returns the (major) tick length, in pixels
 *
 * @return the length in pixels of the major ticks, calculated with the formula
 *         <cite>tickLen * (paint_area.height)</cite>
 *
 * @param paint_area const reference to the current paint area
 * @see paintArea
 */
double QuLinearGaugeBase::getTickLen(const QRectF &paint_area) const
{
    return g_config->tickLen * paint_area.height();
}

/*! \brief returns the free area available for custom drawings within the widget
 *
 * @return the the free area available for custom drawings within the widget
 *
 * @param paint_area const reference to the current paint area
 * @see paintArea
 *
 * \note If the labelPosition property is different from LabelPosition::None,
 *       the label will be drawn inside the freeSpaceRect area. You must call
 *       labelRect before adding custom drawings to the widget to be sure your
 *       drawing does not collide with the label.
 */
QRectF QuLinearGaugeBase::freeSpace(const QRectF &paint_area) const
{
    QRectF free;
    double y1 = gaugeRect(paint_area).bottom();
    free = QRectF(paint_area.left(), y1, paint_area.width(), paint_area.height() - y1);
    return free;
}

/*! \brief returns the bounding rect of the label or an empty rectangle if the label is not visible
 *
 */
QRectF QuLinearGaugeBase::labelRect() const
{
    return d->cache.labelRect;
}

/*! \brief returns the bounding rect of the gauge (union of gauge bar, ticks and text labels)
 *
 * @param paint_area const reference to the current paint area
 * @see paintArea
 *
 * @return the rectangle including the union of the areas occupied by the gauge bar,
 *         the ticks and the text labels of the scale
 */
QRectF QuLinearGaugeBase::gaugeRect(const QRectF& paint_area) const
{
    double y1, gauge_h = getGaugeWidth(paint_area), labhei = d->cache.labelSize.height();
    double tickL = g_config->tickLen * paint_area.height() * 1.2;
    if(d->scaleMode == Normal) {
        y1 =  tickL + labhei;
        if(y1 < gauge_h)
            y1 = gauge_h;
    } else if(d->scaleMode == Inverted) {
        y1 = gauge_h;
    }
    QPointF botRight(paint_area.right(), y1 * 1.01);
    return QRectF(QPointF(0, 0), botRight);
}

void QuLinearGaugeBase::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF &r = paintArea();

    if(height() > width()) {
        p.save();
        p.translate(QPointF(rect().width() + r.left(), 0));
        p.rotate(90);
        p.translate(QPointF(0, r.left()));
    }

    p.setPen(QColor(Qt::lightGray));
    p.drawRect(r);

    if(g_config->drawBackground)
        drawBackground(r, p);

    if(d->labelPosition != NoLabel)
        drawLabel(r, p);

    drawGauge(r, p);

    // draw ticks
    drawTicks(r, p);

    // draw text
    for(int i = 0; g_config->drawText && i < d->cache.labels.size(); i++) {
        drawText(r, i, p);
    }

    // draw needle
    double needle_pos = mapTo(g_config->value_anim, r);
    if(needle_pos >= 0) {
        drawNeedle(needle_pos, r, p);
    }
    if(height() > width())
        p.restore();
}

void QuLinearGaugeBase::drawText(const QRectF &re, int idx, QPainter &p)
{
    QPen pen(g_config->textColor);
    p.setPen(pen);
    p.setBrush(QBrush());
    QFont f = p.font();
    f.setPointSizeF(labelFontSize());
    p.setFont(f);
    QRectF trect = textRect(re, idx);
    QString txt = label(idx);
    p.drawText(trect, Qt::AlignHCenter|Qt::AlignVCenter, txt);
}

/*! \brief Draw the needle
 *
 * This method can be reimplemented to draw custom needles
 *
 * @param end the end of the needle
 * @param angle the angle of the needle, zero at three o'clock
 * @param rect the bounding rect of the drawing area
 * @param p the painter
 *
 * \note In the default implementation, the needle starts from the center of the rect
 */
void QuLinearGaugeBase::drawNeedle(double pos, const QRectF &rect, QPainter &p)
{
    QPen pen;
    QRectF gaugeR = gaugeRect(rect);
    if(indicatorType() == Needle)  {
        double pivrad = g_config->tickLen * rect.height() * g_config->pivotRadius;
        double needle_start, needle_end, h1, lrtop = d->cache.labelRect.top();
        if(d->scaleMode == Inverted) {
            needle_start = gaugeR.bottom();
            needle_end = rect.bottom() - pivrad;
        }
        else {
            needle_start = (g_config->tickLen * rect.height())/2.0; // start drawing arrow at minor tick
            d->labelPosition != NoLabel ? h1 = lrtop - (lrtop-gaugeR.height()) * 0.05 : h1 = rect.top() + rect.height() * 0.75;
            needle_end = h1;
        }
        QRectF pivotRe(pos-pivrad, needle_end-pivrad, 2 * pivrad, 2 * pivrad);
        pen.setColor(g_config->needleGuideColor);
        pen.setWidthF(0.0f);
        p.setPen(pen);
        p.drawLine(rect.left() + 1.8*pivrad, pivotRe.center().y(), pos, pivotRe.center().y());
        QBrush b(g_config->arrowColor);
        pen.setColor(g_config->needleColor);

        pen.setWidthF(g_config->arrowPenWidth);
        p.setBrush(b);
        p.setPen(pen);
        p.drawLine(QPointF(pos, rect.top()), QPointF(pos, needle_end));
        // draw arrow
        double arrlen = g_config->tickLen * rect.height() * g_config->arrowSize; //
        QVector<QPointF> points;
        double deg = 30.0;
        double a_rad = deg * M_PI / 180.0;
        QPointF atop(pos, needle_start);
        QPointF bl(pos - arrlen * sin(a_rad), atop.y() + arrlen * cos(a_rad));
        QPointF br(pos + arrlen * sin(a_rad), bl.y());
        points << atop << bl << br;
        QPolygonF ar(points);
        p.drawPolygon(ar);
        QRadialGradient rg(pivotRe.center(), pivrad);
        rg.setColorAt(0, QColor(Qt::darkGray));
        rg.setColorAt(1, QColor(Qt::white));
        QBrush bru(rg);
        p.setBrush(bru);
        p.setPen(QColor(Qt::darkGray));
        p.drawEllipse(pivotRe);
    }
    else if(indicatorType() == Thermo) {
        double y1 = gaugeR.bottom();
        QRect thermoRect(rect.left(), y1 * 1.02, pos - rect.left(), getThermoWidth(rect));
        QLinearGradient lg(QPointF(thermoRect.center().x(), thermoRect.top() ),
                           QPointF(thermoRect.center().x(), thermoRect.top() + thermoRect.height()));
        lg.setColorAt(0.0, g_config->thermoColor2);
        lg.setColorAt(0.3, g_config->thermoColor1);
        lg.setColorAt(0.7, g_config->thermoColor1);
        lg.setColorAt(1, g_config->thermoColor2);

        QBrush bru(lg);
        QPen pe;
        pe.setColor(g_config->thermoPenColor);
        pe.setWidthF(0.0);

        p.setPen(pe);
        p.setBrush(bru);
        p.drawRect(thermoRect);
    }
}

void QuLinearGaugeBase::drawGauge(const QRectF &rect,  QPainter &p)
{
    const QRectF& pa = rect;
    double xle = pa.left(), xlw = xle, xhw = pa.right(),  xhe = xhw;
    double ga_w = g_config->gaugeWidth * rect.height();
    bool error_ok = g_config->low_e < g_config->high_e && g_config->low_e >= g_config->min && g_config->high_e <= g_config->max;

    if(d->readError) {
        p.setPen(QPen(g_config->readErrorColor.darker()));
        p.setBrush(QBrush(g_config->readErrorColor));
    }
    else {

        p.setPen(QColor(Qt::transparent));
        if(error_ok) {
            // draw err color from min to low err to low warn
            xle = mapTo(g_config->low_e, pa);
            xhe = mapTo(g_config->high_e, pa);
            p.setBrush(QBrush(g_config->errorColor));
            p.drawRect(pa.left(), pa.top(), xle-pa.left(), ga_w);
            p.drawRect(xhe, pa.top(), xhw - xhe, ga_w); // xhw either initialised to pa.right or pa.top
        }
        bool warning_ok = g_config->low_w < g_config->high_w && g_config->low_w >= g_config->min && g_config->high_w <= g_config->max;
        if( (warning_ok && error_ok && g_config->low_w > g_config->low_e && g_config->high_w < g_config->high_e) ||
                (warning_ok && !error_ok) ) {
            p.setBrush(QBrush(g_config->warningColor));
            xlw = mapTo(g_config->low_w, pa);
            xhw = mapTo(g_config->high_w, pa);
            p.drawRect(xle, pa.top(), xlw - xle, ga_w);
            p.drawRect(xhw, pa.top(), xhe - xhw, ga_w);
        }
        p.setBrush(QBrush(g_config->normalColor));
    }
    p.drawRect(qMax(xle, xlw), pa.top(), qMin(xhe, xhw) - qMax(xle, xlw), ga_w);
}

void QuLinearGaugeBase::drawTicks(const QRectF &r, QPainter &p)
{
    QPen pen = p.pen();
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(0.0);
    double maxsiz = r.width(), minsiz = r.height();
    double step = (g_config->max - g_config->min) / (double) ( (g_config->minorTicksCount + 1) * (g_config->ticksCount -1) ) ;
    double val = g_config->min;
    double tickLen;
    double y1, invert_factor, start = r.left();
    if(d->scaleMode == Normal) {
        y1 = r.top();
        invert_factor = 1.0;
    } else if(d->scaleMode == Inverted) {
        y1 = r.top() + r.height() * g_config->gaugeWidth;
        invert_factor = -1.0;
    }
    const double interval = (double) maxsiz / (double) (g_config->ticksCount - 1);
    for(int i = 0; i < ticksCount(); i++) {
        double x = start;
        for(int j = 0; (i < g_config->ticksCount -1 && j < g_config->minorTicksCount + 1) || j < 1; j++) {
            j == 0 ? tickLen = minsiz * g_config->tickLen : tickLen = minsiz * g_config->tickLen /2.0;

            QPointF p0 = QPointF(x, y1);
            QPointF p1 = QPointF(x, y1 + invert_factor * tickLen);
            if(g_config->drawColoredTicks) {
                if(inWarningRange(val))
                    pen.setColor(g_config->warningColor);
                else if(inErrorRange(val))
                    pen.setColor(g_config->errorColor);
                else
                    pen.setColor(g_config->normalColor);

                p.setPen(pen);
                p.drawLine(p0, p1);
            }
            if(g_config->drawTickCore1 || g_config->drawTickCore2) {
                QPen corePen(g_config->tickCoreColor);
                corePen.setWidthF(pen.widthF()/2.0);
                corePen.setCapStyle(Qt::FlatCap);
                p.setPen(corePen);
                if(g_config->drawTickCore1 && g_config->drawTickCore2) {
                    p.drawLine(p0, p1);
                }
                else if(g_config->drawTickCore1 && !g_config->drawTickCore2) {
                    p1 = QPointF(x, y1 + invert_factor * g_config->gaugeWidth * r.height());
                    p.drawLine(p0, p1);
                }
                else if(!g_config->drawTickCore1 && g_config->drawTickCore2) {
                    p0 = QPointF(x, y1 + g_config->gaugeWidth * r.height());
                    p1 = QPointF(x, p0.y() + invert_factor * tickLen);
                    p.drawLine(p0, p1);
                }
            }

            x += interval / static_cast<double> (g_config->minorTicksCount + 1);
            val += step;
        }
        start += interval;
        //  val += step;
    }
}

void QuLinearGaugeBase::drawBackground(const QRectF &rect, QPainter &p)
{
    QPointF start(rect.center().x(), rect.bottom());
    QPointF end = rect.center();
    QLinearGradient rg(start, end);
    QColor c0 = g_config->backgroundColor;
    // use current value instead of the value dedicated to the animation
    // so that the background immediately reflects the value range
    QColor c1 = valueColor(g_config->value);
    c0.setAlpha(g_config->backgroundColorAlpha);
    c1.setAlpha(g_config->backgroundColorAlpha);
    rg.setColorAt(g_config->backgroundGradientSpread, c1);
    rg.setColorAt(0, c0);
    QBrush br = QBrush(rg);
    p.setBrush(br);
    p.drawRect(rect);
}

void QuLinearGaugeBase::drawLabel(const QRectF &rect, QPainter& p)
{
    QRectF area = freeSpace(rect);
    QPen pen(Qt::black);
    p.setPen(pen);

    QFont f = p.font();
    // start with suggested point size, then shrink if necessary
    f.setPointSizeF(d->cache.labelPointSize * d->labelFontScale);
    QFontMetrics fm(f);
    double h = area.height() - getThermoWidth(rect);
    while(fm.height() > h && f.pointSizeF() >= 4) {
        f.setPointSizeF(f.pointSizeF() - 0.5);
        fm = QFontMetrics(f);
    }
    // set the font on the painter
    p.setFont(f);
    QString txt;
    if(d->label.isEmpty()) {
        txt = formatLabel(g_config->value, d->labelValueFormat.toStdString().c_str());
        if(!g_config->unit.isEmpty())
            txt += " [" + g_config->unit + "]";
    }
    else
        txt = d->label;

    int w = fm.horizontalAdvance(txt);
    double x, y;
    switch (d->labelPosition) {
    case West:
        x = area.left() + area.width() * 0.02;
        break;
    case East:
        x = area.left() + area.width() * 0.98 - w;
        break;
    case Center:
        x = area.left() + area.width() * 0.5 - w/2.0;
        break;
    default:
        break;
    }
    y = area.bottom() - 1.05 * fm.height();
    QRectF textR(x, y, w, fm.height());
    p.drawText(textR, Qt::AlignHCenter|Qt::AlignVCenter, txt);
    d->cache.labelRect = textR;
}


void QuLinearGaugeBase::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::FontChange)
        regenerateCache();
    QWidget::changeEvent(e);
}

void QuLinearGaugeBase::resizeEvent(QResizeEvent *re)
{
    QWidget::resizeEvent(re);
    regenerateCache(); //  updateLabelsCache() + updateLabelsFontSize() + updatePaintArea
}

QSize QuLinearGaugeBase::sizeHint() const
{
    return QWidget::sizeHint();
}

QSize QuLinearGaugeBase::minimumSizeHint() const
{
    QFont f = font();
    f.setPointSizeF(g_config->minFontSize);
    QFontMetrics fm(f);
    int txtmaxwid = fm.horizontalAdvance(d->cache.longestLabel);
    int txtmaxhei = fm.height();
    int label_space;
    if(d->labelPosition != NoLabel) {
        f.setPointSizeF(f.pointSize() * d->labelFontScale);
        fm = QFontMetrics(f);
        label_space = fm.height() * 1.2;
    }
    else
        label_space = 0;
    const double wfactor = 1.1;
    const int w = txtmaxwid * g_config->ticksCount * wfactor;
    const int h = 2 * txtmaxhei + label_space;
    QSize siz;
    orientation() == Qt::Horizontal ? siz = QSize(w, h) : siz = QSize(h, w);
    return siz;
}

void QuLinearGaugeBase::updatePaintArea()
{
    QRect wr = this->rect();
    // size not changed: use old rect
    if(wr == d->cache.oldRect)
        return;

    d->cache.oldRect = wr;
    double len;
    orientation() == Qt::Horizontal ? len = wr.width() : len = wr.height();
    double dx = qMax(len * 0.02, d->cache.labelSize.width()/2.0);
    QRectF& a = d->cache.paintArea;
    orientation() == Qt::Horizontal ? a = wr.adjusted(dx, 0, -dx, 0) : // a = wr.adjusted(0, dx, 0, -dx);
            a = QRectF(dx, 0, wr.height()-2*dx, wr.width());

}

