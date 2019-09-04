#include "qucirculargaugebase.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QtDebug>
#include <cumacros.h>
#include <math.h>

class QuGaugeCache {
public:
    QuGaugeCache() {
        invalidate();
    }

    QList<int> labelsDistrib;
    QString longestLabel;
    // text currently displayed in the label inside the gauge
    // used to recalculate font size when the length of the text
    // changes
    QString label_txt;
    QStringList labels;
    QRectF paintArea;
    QRectF oldRect;
    QSize labelSize;
    double labelPtSiz;
    double textLabelPtSiz; // pixel size of the optional text label

    void invalidate(){
        labelsDistrib.clear();
        longestLabel = QString();
        paintArea = oldRect = QRectF();
        labelPtSiz = 10;
        textLabelPtSiz = -1;
        labelSize = QSize();
    }

    bool isValid() const {
        return labelsDistrib.size() == 4 && longestLabel.length() > 0;
    }
};

class QuCircularGaugeBasePrivate {
public:
    QString label;   // text property. If empty, value is displayed
    QString labelValueFormat; // if label displays value, use this format
    int startAngle, spanAngle;
    QSize minSizeHint, sizeHint;
    QuCircularGaugeBase::Quads quad;
    double sin_a0, sin_a1, cos_a0, cos_a1;
    QuGaugeCache cache;
    QPropertyAnimation *propertyAnimation;
    QDateTime lastValueDateTime;
    QuCircularGaugeBase::LabelPosition labelPosition;
    double labelDistFromCenter, labelFontScale;
    bool readError;
};

QuCircularGaugeBase::QuCircularGaugeBase(QWidget *parent) : QWidget(parent)
{
    g_config = new QuGaugeConfig();
    d = new QuCircularGaugeBasePrivate;
    d->startAngle = 120; // from 3 o' clock
    d->spanAngle = 300;
    g_config->pivotRadius = 0.08; // 8% of the radius
    d->propertyAnimation = NULL;
    d->labelPosition = South;
    d->labelDistFromCenter = 0.25;
    d->labelValueFormat = "%.2f";
    d->labelFontScale = -1.0;
    d->readError = false;
    m_anglesUpdate();
}

QuCircularGaugeBase::~QuCircularGaugeBase()
{
    if(d->propertyAnimation) {
        d->propertyAnimation->stop();
        delete d->propertyAnimation;
    }
    delete d;
    delete g_config;
}

double QuCircularGaugeBase::value() const
{
    return g_config->value;
}

/*! \brief returns the value during the animation
 *
 * To get the actual value, please refer to the QuCircularGaugeBase::value method
 */
double QuCircularGaugeBase::value_anim() const
{
    return g_config->value_anim;
}

/*! \brief returns the maximum value represented by this gauge
 *
 * @return the maximum value of this gauge's scale
 */
double QuCircularGaugeBase::maxValue() const
{
    return g_config->max;
}

/*! \brief returns the minimum value represented by this gauge
 *
 * @return the minimum value of this gauge's scale
 */
double QuCircularGaugeBase::minValue() const
{
    return g_config->min;
}

/*! \brief returns the span of the angle of the circular gauge, in degrees
 *
 * @return the span angle in degrees.
 *
 * \par Note
 * According to Qt documentation, negative values draw the arc clockwise
 *
 * @see angleStart
 * @see setAngleSpan
 */
int QuCircularGaugeBase::angleSpan() const
{
    return d->spanAngle;
}

/*! \brief returns the angle from which the gauge arc starts
 *
 * @return the angle, in degrees, where the gauge arc starts
 *
 * @see angleSpan
 * @see setAngleStart
 */
int QuCircularGaugeBase::angleStart() const
{
    return d->startAngle;
}

/*! \brief returns the number of *major* ticks drawn in the arc
 *
 * @return the number of *major ticks* drawn
 *
 * @see minorTicksCount
 */
int QuCircularGaugeBase::ticksCount() const
{
    return g_config->ticksCount;
}

/*! \brief returns the number of *minor* ticks drawn between each couple of major ticks
 * @return the number of minor ticks between two subsequent major ticks
 *
 * @see ticksCount
 * @see setMinorTicksCount
 */
int QuCircularGaugeBase::minorTicksCount() const
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
double QuCircularGaugeBase::tickLen() const
{
    return g_config->tickLen;
}

/*! \brief returns the *pen width* used to draw the gauge arc
 *
 * @return the pen width, in pixels, floating point, used to draw the arc
 *
 * \par Notes
 * \li If the drawTickCore1 property is enabled, a tick is drawn inside the arc and its length
 * is equal to gaugeWidth.
 * \li If the drawTickCore2 property is enabled a tick is drawn from the arc bottom side and it
 * will be gaugeWidth pixels long
 *
 * @see drawTickCore1
 * @see drawTickCore2
 * @see drawColoredTicks
 *
 */
double QuCircularGaugeBase::gaugeWidth() const
{
    return g_config->gaugeWidth;
}

/*! \brief If greater than zero, a circle is drawn on the pivot of the needle
 *
 * This number, a floating point in the interval [0.0, 1.0], represents the fraction of the
 * length of the radius used to draw a circle on the pivot of the needle.
 *
 * The formula used to calculate the pivot circle radius is:
 *
 * \code
 * qMin(paintArea().width(), paintArea().height())/2.0 * g_config->pivotRadius
 * \endcode
 *
 * @see getPivotRadius
 */
double QuCircularGaugeBase::pivotCircleRadius() const
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
bool QuCircularGaugeBase::drawBackgroundEnabled() const {
    return g_config->drawBackground;
}

/*! \brief either draw or not the labels with the values associated to each major tick
 *
 * @return true the value of each major tick is drawn
 *         false no values are drawn on the gauge
 *
 * \note if you want the current value to be displayed on a separate bigger label somewhere
 *       in the widget area, use the labelPosition property and set it to a value different
 *       from QuCircularBase::NoLabel. At the same time, the label property must be set to
 *       an empty string.
 *
 * @see label
 * @see setLabelPosition
 * @see setLabel
 */
bool QuCircularGaugeBase::drawText() const
{
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
bool QuCircularGaugeBase::drawTickCore1() const
{
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
bool QuCircularGaugeBase::drawTickCore2() const
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
bool QuCircularGaugeBase::drawColoredTicks() const
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
bool QuCircularGaugeBase::animationEnabled() const
{
    return g_config->animationEnabled;
}

bool QuCircularGaugeBase::readError() const
{
    return d->readError;
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
qint64 QuCircularGaugeBase::maxAnimationDuration() const
{
    return g_config->maximumAnimDuration;
}

QuCircularGaugeBase::LabelPosition QuCircularGaugeBase::labelPosition() const
{
    return d->labelPosition;
}

double QuCircularGaugeBase::labelDistFromCenter() const
{
    return d->labelDistFromCenter;
}

/*! \brief scales the label font respect to the automatically calculated size
 *
 * @return the label font scale value, between 0 and 2.
 *
 * The font size of the text on the gauge is scaled by this factor
 * respect to the automatically calculated size
 *
 * \note The default value is -1, meaning label font is automatically calculated
 */
double QuCircularGaugeBase::labelFontScale() const
{
    return d->labelFontScale;
}

void QuCircularGaugeBase::setLowWarning(double w) {
    g_config->low_w = w;
    update();
}

double QuCircularGaugeBase::lowWarning() {
    return g_config->low_w;
}

void QuCircularGaugeBase::setHighWarning(double w) {
    g_config->high_w = w;
    update();
}

double QuCircularGaugeBase::highWarning() {
    return g_config->high_w;
}

double QuCircularGaugeBase::lowError(){
    return g_config->low_e;
}

QColor QuCircularGaugeBase::textColor() const
{
    return g_config->textColor;
}

QColor QuCircularGaugeBase::warningColor() const
{
    return g_config->warningColor;
}

QColor QuCircularGaugeBase::errorColor() const
{
    return g_config->errorColor;
}

QColor QuCircularGaugeBase::normalColor() const
{
    return g_config->normalColor;
}

QColor QuCircularGaugeBase::readErrorColor() const
{
    return g_config->readErrorColor;
}

QColor QuCircularGaugeBase::backgroundColor() const
{
    return g_config->backgroundColor;
}

QColor QuCircularGaugeBase::needleColor() const
{
    return g_config->needleColor;
}

QColor QuCircularGaugeBase::tickCoreColor() const
{
    return g_config->tickCoreColor;
}

double QuCircularGaugeBase::backgroundGradientSpread() const
{
    return g_config->backgroundGradientSpread;
}

int QuCircularGaugeBase::backgroundColorAlpha() const
{
    return g_config->backgroundColorAlpha;
}

void QuCircularGaugeBase::setLowError(double e) {
    g_config->low_e = e;
    update();
}

void QuCircularGaugeBase::setTextColor(const QColor &c)
{
    g_config->textColor = c;
    update();
}

void QuCircularGaugeBase::setErrorColor(const QColor &ec)
{
    g_config->errorColor = ec;
    update();
}

void QuCircularGaugeBase::setWarningColor(const QColor &wc)
{
    g_config->warningColor = wc;
    update();
}

void QuCircularGaugeBase::setNormalColor(const QColor &nc)
{
    g_config->normalColor = nc;
    update();
}

void QuCircularGaugeBase::setReadErrorColor(const QColor &rec)
{
    g_config->readErrorColor = rec;
    update();
}

void QuCircularGaugeBase::setBackgroundColor(const QColor &bc)
{
    g_config->backgroundColor = bc;
    update();
}

void QuCircularGaugeBase::setNeedleColor(const QColor &c)
{
    g_config->needleColor = c;
    update();
}

void QuCircularGaugeBase::setTickCoreColor(const QColor &c)
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
void QuCircularGaugeBase::setBackgroundGradientSpread(double s)
{
    if(s > 0.0 && s <= 1) {
        g_config->backgroundGradientSpread = s;
        update();
    }
    else
        perr("QuCircularGaugeBase::setBackgroundGradientSpread: value must be 0 < value <= 1");
}

void QuCircularGaugeBase::setBackgroundColorAlpha(int a)
{
    g_config->backgroundColorAlpha = a;
    update();
}

void QuCircularGaugeBase::setDrawTickCore1(bool t)
{
    g_config->drawTickCore1 = t;
    update();
}

void QuCircularGaugeBase::setDrawTickCore2(bool t)
{
    g_config->drawTickCore2 = t;
    update();
}

void QuCircularGaugeBase::setDrawColoredTicks(bool dr)
{
    g_config->drawColoredTicks = dr;
    update();
}

void QuCircularGaugeBase::setDrawText(bool dt)
{
    g_config->drawText = dt;
    update();
}

void QuCircularGaugeBase::setAnimationEnabled(bool ae)
{
    g_config->animationEnabled = ae;
    update();
}

void QuCircularGaugeBase::setMaxAnimationDuration(qint64 millis)
{
    if(millis > 0)
        g_config->maximumAnimDuration = millis;
}

void QuCircularGaugeBase::setLabelPosition(QuCircularGaugeBase::LabelPosition p)
{
    d->labelPosition = p;
    update();
}

void QuCircularGaugeBase::setLabelDistFromCenter(double radius_percent)
{
    if(radius_percent >= 0 && radius_percent <= 1.0) {
        d->labelDistFromCenter = radius_percent;
        update();
    }
    else
        perr("QuCircularGaugeBase::setLabelDistFromCenter: value must be 0 <= value <= 1.0");
}

void QuCircularGaugeBase::setLabelValueFormat(const QString &f)
{
    d->labelValueFormat = f;
    update();
}

void QuCircularGaugeBase::setUnit(const QString &u)
{
    g_config->unit = u;
    update();
}

/*! \brief changes the label font size  respect to the automatically calculated size
 *
 * @param factor the label font scale value
 *
 * The font size of the text on the gauge is scaled by this factor respect to the automatically calculated size
 *
 * \note The default value is -1
 */
void QuCircularGaugeBase::setLabelFontScale(double factor)
{
    d->labelFontScale = factor;
    update();
}

void QuCircularGaugeBase::setHighError(double e) {
    g_config->high_e = e;
    update();
}

double QuCircularGaugeBase::highError() {
    return g_config->high_e;
}

double QuCircularGaugeBase::getPivotRadius()
{
    return qMin(paintArea().width(), paintArea().height())/2.0 * g_config->pivotRadius;
}

QString QuCircularGaugeBase::label() const
{
    return d->label;
}

QString QuCircularGaugeBase::format() const
{
    return g_config->format;
}

bool QuCircularGaugeBase::formatFromPropertyEnabled() const
{
    return g_config->format_from_property;
}

QString QuCircularGaugeBase::getAppliedFormat() const {
    if(g_config->format_from_property)
        return g_config->format_prop;
    return g_config->format;
}

QString QuCircularGaugeBase::labelValueFormat() const
{
    return d->labelValueFormat;
}

QString QuCircularGaugeBase::unit() const
{
    return g_config->unit;
}

/*! \brief maps a value to a point close to the arc of the gauge
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
QPointF QuCircularGaugeBase::mapTo(double val, int *angle, double radius, const QRectF& paint_area)
{
    QPointF p;
    if(angle)
        *angle = -1;
    QRectF paintA(paint_area);
    if(paint_area == QRectF())
        paintA = paintArea();
    if(radius <= 0)
        radius = paintA.width() / 2;

    double range = g_config->max - g_config->min;
    double fullscale_factor = 0.0;
    if(g_config->max >= g_config->min) {
        if(val < g_config->min) {
            val = g_config->min;
            fullscale_factor = -0.01;
        }
        else if(val > g_config->max) {
            val = g_config->max;
            fullscale_factor = 0.01;
        }

        double a = d->startAngle;
        // allow arc > 360, do not apply modulus 360 to it
        double arc = qRound(a + d->spanAngle * (val - g_config->min) / range);
        arc = arc * (1 + fullscale_factor);
        if(angle)
            *angle = static_cast<int>(arc);
        arc = arc * M_PI / 180.0;
        p = QPointF(paintA.x() + paintA.width() / 2.0 + radius * (1.0 - g_config->tickLen * 1.2) * cos(arc),
                    paintA.y() + paintA.height() / 2.0 + radius * (1.0 - g_config->tickLen * 1.2) * sin(arc));

    }
    return p;
}

QRectF QuCircularGaugeBase::textRect(int tick, double radius)
{
    int idx = 0;
    double step = (g_config->max - g_config->min) / (g_config->ticksCount - 1);
    double val = g_config->min;
    while(val <= g_config->max) {
        if(idx == tick) {
            QPointF labelP = mapTo(val, NULL, radius);
            QRectF trect(labelP, d->cache.labelSize);
            trect.moveLeft(trect.left() - trect.width()/2.0);
            trect.moveTop(trect.top() - trect.height()/2.0);
            return trect;
        }
        val += step;
        idx++;
    }
    return QRectF();
}

double QuCircularGaugeBase::labelFontSize()
{
    if(!d->cache.isValid())
        regenerateCache();
    return d->cache.labelPtSiz;
}

QString QuCircularGaugeBase::formatLabel(double value, const QString& format) const
{
    char clab[32];
    snprintf(clab, 32, format.toStdString().c_str(), value);
    return QString(clab);
}

QString QuCircularGaugeBase::label(int i) const
{
    if(i < d->cache.labels.size())
        return d->cache.labels[i];
    return QString();
}

void QuCircularGaugeBase::setValue(double v)
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
    else
        g_config->value_anim = v;

    g_config->value = v;
    d->lastValueDateTime = now;

}

void QuCircularGaugeBase::setReadError(bool err)
{
    d->readError = err;
    update();
}

void QuCircularGaugeBase::setValue_anim(double v)
{
    g_config->value_anim = v;
    update();
}

/*! \brief set the start angle of the arc, in degrees, from 6 o'clock
 *
 * @param as the value of the angle to use as starting point for the drawing of the gauge's arc, in degrees,
 *        from 6 o'clock. By default, the span angle is 300 degrees and the start is set to 30 degrees from
 *        3 o'clock
 *
 * @see setAngleSpan
 */
void QuCircularGaugeBase::setAngleStart(int as)
{
    d->startAngle = as;
    m_anglesUpdate();
    regenerateCache();
    update();
}

/*! \brief set the span of the arc of the gauge
 *
 * @param the angle span, in degrees. The default is 300 degrees from angleStart (zero at 3 o'clock)
 *
 * @see setAngleStart
 */
void QuCircularGaugeBase::setAngleSpan(int a)
{
    d->spanAngle = a;
    m_anglesUpdate();
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setMinValue(double min)
{
    g_config->min = min;
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setMaxValue(double max)
{
    g_config->max = max;
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setLabel(const QString &l)
{
    d->label = l;
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setTicksCount(int t)
{
    g_config->ticksCount = t;
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setMinorTicksCount(int t)
{
    g_config->minorTicksCount = t;
    update();
}

/** \brief Set the tick len as percentage of the radius of the circle
 *
 * @param tl a value between 0 and 1 representing the length of a tick with respect
 *        to the length of the radius
 */
void QuCircularGaugeBase::setTickLen(double tl)
{
    g_config->tickLen = tl;
    regenerateCache();
    update();
}

void QuCircularGaugeBase::setGaugeWidth(double w)
{
    g_config->gaugeWidth = w;
    update();
}

void QuCircularGaugeBase::setPivotCircleRadius(double percent)
{
    if(percent >= 0 && percent  <= 1.0) {
        g_config->pivotRadius = percent;
        updatePaintArea();
        regenerateCache();
        update();
    }
    else
        perr("QuCircularGaugeBase::setPivotCircleRadius: pivot radius must be a number between 0 and 1");
}

void QuCircularGaugeBase::setDrawBackgroundEnabled(bool en)
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
 * \par note
 * This method is called by QuCircularGauge when a CuData has the value "property" associated to the key "type"
 * (configuration (or property) data).
 *
 * @see setFormatFromPropertyEnabled
 */
void QuCircularGaugeBase::setFormat(const QString &f)
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
void QuCircularGaugeBase::setFormatFromPropertyEnabled(bool ffp)
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
 * This method is called by QuCircularGauge when a CuData which key "type" has the "property" value.
 *
 * @see setFormatFromPropertyEnabled
 */
void QuCircularGaugeBase::setFormatProperty(const QString &fp)
{
    g_config->format_prop = fp;
    regenerateCache();
    update();
}

/*! \brief get the labels distribution according to the shape of the gauge
 *
 * @return a list of integers as follows
 * \li index 0: number of labels from 6:01 to 9:00 o'clock
 * \li index 1: number of labels from 9:01 to 12:00 o'clock
 * \li index 2: number of labels from 12:01 to 3.00 o'clock
 * \li index 3: number of labels from 3:01 to 6:00
 *
 */
QList<int> QuCircularGaugeBase::updateLabelsDistrib() const
{
    QList<int> ld =  QList<int>() << 0 << 0 << 0 << 0;
    QRect r(0, 0, 100, 100); // sample rect
    QPointF topL = r.topLeft();
    double radius = r.width()/2.0;
    double a = d->startAngle * M_PI / 180.0;
    double arc = (double) d->spanAngle / (double) (g_config->ticksCount - 1) * M_PI / 180.0;
    QPointF center_draw = r.center();
    for(int i = 0; i < ticksCount(); i++) {
        QPointF p = QPointF(topL.x() + r.width() / 2.0 + radius * cos(a), topL.y() + r.height() / 2.0 + radius * sin(a));
        if(!d->cache.isValid()) {
            if(p.x() <= center_draw.x() && p.y() > center_draw.y())
                ld[0]++;
            else  if(p.x() <= center_draw.x() && p.y() <= center_draw.y())
                ld[1]++;
            else if(p.x() > center_draw.x() && p.y() <= center_draw.y())
                ld[2]++;
            else
                ld[3]++;
        }

        a += arc;
    }
    return ld;
}

void QuCircularGaugeBase::updateLabelsCache()
{
    QFontMetrics fm(font());
    d->cache.labels.clear();
    double step = (g_config->max - g_config->min) / (g_config->ticksCount - 1);
    double val = g_config->min;
    while(val <= g_config->max) {
        QString lab = formatLabel(val, getAppliedFormat());
        if(fm.width(lab) > fm.width(d->cache.longestLabel))
            d->cache.longestLabel = lab;
        // fill labels cache
        d->cache.labels << lab;
        val += step;
    }
    updateGeometry();
}

void QuCircularGaugeBase::regenerateCache()
{
    d->cache.invalidate();
    updateLabelsCache();
    d->cache.labelsDistrib = updateLabelsDistrib();
    updateLabelsFontSize();
}

void QuCircularGaugeBase::updateLabelsFontSize() {
    QFont f = font();
    QFontMetrics fm(f);
    double w = fm.width(d->cache.longestLabel), rw;
    QList<int> ld = d->cache.labelsDistrib;

    if(ld.size() == 4) {
        QRectF p_a = paintArea();
        double maxlw_upper = qMax(ld[1], ld[2]);
        double maxlw_lower = qMax(ld[0], ld[3]);
        rw = qMin(qRound(p_a.height()), qRound(p_a.width())) / 2 / qMax(maxlw_lower, maxlw_upper);
        while( w < rw && f.pointSizeF() > 4) {
            f.setPointSizeF(f.pointSizeF() + 0.5);
            w = QFontMetrics(f).width(d->cache.longestLabel);
        }
        while(w > rw && f.pointSizeF() > 4) {
            f.setPointSizeF(f.pointSizeF() - 0.5);
            w = QFontMetrics(f).width(d->cache.longestLabel);
        }
    }

    d->cache.labelPtSiz = f.pointSizeF();
    fm = QFontMetrics(f);
    d->cache.labelSize = QSize(fm.width(d->cache.longestLabel), fm.height());
}
QRectF QuCircularGaugeBase::paintArea() {
    if(!d->cache.paintArea.isValid())
        updatePaintArea();
    return d->cache.paintArea;
}

void QuCircularGaugeBase::m_anglesUpdate()
{
    double a0 = d->startAngle * M_PI / 180.0;
    double a1 = (d->startAngle + d->spanAngle) * M_PI / 180.0;
    d->sin_a0 = sin(a0);
    d->sin_a1 = sin(a1);
    d->cos_a0 = cos(a0);
    d->cos_a1 = cos(a1);

    cuprintf("m_anglesUpdate a0 = %d a1 = %d \e[1;34msin a0 %f cos a0 %f \e[1;33m sin a1 %f cos a1 %f\e[0m\n",
             d->startAngle, d->startAngle + d->spanAngle, d->sin_a0, d->cos_a0, d->sin_a1, d->cos_a1);

    if(d->sin_a0 < 0 && d->cos_a0 >= 0 && d->sin_a1 <= 0 && d->cos_a1 >= 0)
        d->quad = q1;
    else if(d->sin_a0 < 0 && d->sin_a1 <= 0 && d->cos_a0 < 0 && d->cos_a1 < 0)
        d->quad = q2;
    else if(d->sin_a0 >= 0 && d->sin_a1 >= 0 && d->cos_a0 < 0 && d->cos_a1 < 0)
        d->quad = q3;
    else if(d->sin_a0 >= 0 && d->cos_a0 > 0 && d->sin_a1 >= 0 && d->cos_a1 > 0)
        d->quad = q4;

    else if(d->sin_a0 <=0 && d->cos_a0 <= 0 && d->sin_a1 <=0 && d->cos_a1 > 0)
        d->quad = q21;
    else if(d->sin_a0 >= 0 && d->cos_a0 >=0 && d->sin_a1 >=0 && d->cos_a1 <=0)  // quad 4, 3
        d->quad = q43;
    else if( d->sin_a0 >0 && d->cos_a0 <= 0 && d->sin_a1 < 0 && d->cos_a1 <= 0) // quad 3,2
        d->quad = q32;
    else if(d->sin_a0 <= 0 && d->cos_a0 > 0 && d->sin_a1 > 0 && d->cos_a1 > 0)
        d->quad = q14;

    //  q321, q214, q143, q432
    else if(d->sin_a0 > 0 && d->cos_a0 < 0 && d->sin_a1 < 0 && d->cos_a1 > 0)
        d->quad = q321;
    else if(d->sin_a0 < 0 && d->cos_a0 < 0 && d->sin_a1 >0 && d->cos_a1 > 0)
        d->quad = q214;
    else if(d->sin_a0 < 0 && d->cos_a0 > 0 && d->sin_a1 > 0 && d->cos_a1 < 0)
        d->quad = q143;
    else if(d->sin_a0 > 0 && d->cos_a0 > 0 && d->sin_a1 < 0 && d->cos_a1 < 0)
        d->quad = q432;
    else if(d->sin_a0 > 0 && d->cos_a0 > 0 && d->sin_a1 < 0 && d->cos_a1 > 0)
        d->quad = q4321;
    else if(d->sin_a0 > 0 && d->cos_a0 < 0 && d->sin_a1 > 0 && d->cos_a1 > 0)
        d->quad = q3214;
    else if(d->sin_a0 < 0 && d->cos_a0 < 0 && d->sin_a1 > 0 && d->cos_a1 < 0)
        d->quad = q2143;
    else
        d->quad = q1432;
}

double QuCircularGaugeBase::m_getMarginW(double radius)
{
    QFont fo = font();
    fo.setPointSizeF(d->cache.labelPtSiz);
    QFontMetrics fme(fo);
    double margin_w = fme.width(d->cache.longestLabel) / 2;
    margin_w = qMax(margin_w, radius * g_config->pivotRadius);
    return margin_w;
}

double QuCircularGaugeBase::m_getMarginH(double radius) {
    QFont fo = font();
    fo.setPointSizeF(d->cache.labelPtSiz);
    QFontMetrics fme(fo);
    double margin_h = static_cast<double>(fme.height());
    margin_h = qMax(margin_h, radius * g_config->pivotRadius);
    return margin_h;
}


bool QuCircularGaugeBase::inWarningRange(double val) const
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

bool QuCircularGaugeBase::inErrorRange(double val) const
{
    if(g_config->low_e == g_config->high_e)
        return false;
    return val <= g_config->low_e || val >= g_config->high_e;
}

QColor QuCircularGaugeBase::valueColor(double val) const
{
    QColor c;
    if(inWarningRange(val)) return g_config->warningColor;
    else if(inErrorRange(val)) return g_config->errorColor;
    else return g_config->normalColor;
}

void QuCircularGaugeBase::paintEvent(QPaintEvent *pe)
{
    QWidget::paintEvent(pe);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // draw the gauge arc
    const QRectF r = paintArea();
    if(!r.isValid())
        return;

    p.setPen(QColor(Qt::lightGray));
    //  p.drawRect(r);

    if(g_config->drawBackground)
        drawBackground(r, p);

    if(d->labelPosition != NoLabel)
        drawLabel(r, p);

    drawGauge(r, d->startAngle, d->spanAngle, p);

    // draw ticks
    drawTicks(r, p);

    // draw text
    for(int i = 0; g_config->drawText && i < d->cache.labels.size(); i++) {
        drawText(r, i, p);
    }

    // draw needle
    int angle;
    QPointF end = mapTo(g_config->value_anim, &angle);
    if(!end.isNull()) {
        drawNeedle(end, angle, r, p);
    }

    drawPivot(r, p);
}

void QuCircularGaugeBase::drawText(const QRectF &rect, int idx, QPainter &p)
{
    Q_UNUSED(rect)
    QPen pen(g_config->textColor);
    p.setPen(pen);
    QFont f = p.font();
    f.setPointSizeF(labelFontSize());
    p.setFont(f);
    QRectF trect = textRect(idx, paintArea().width()/2.0 * (1-g_config->tickLen) * 0.9);
    QString txt = label(idx);
    p.drawText(trect, Qt::AlignHCenter|Qt::AlignVCenter, txt);
    //    p.drawRect(trect);
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
void QuCircularGaugeBase::drawNeedle(const QPointF &end, int angle, const QRectF &rect, QPainter &p)
{
    QPolygonF po;
    int centerAngle = 20;
    int radius = rect.width() / 2.0;
    int smallSide;
    if(g_config->pivotRadius > 0)
        smallSide = getPivotRadius();
    else
        smallSide = radius * 0.08;
    double arc = (angle + centerAngle) *  M_PI / 180.0;
    QPointF p2, p1 = QPointF(rect.left() + rect.width() / 2.0 + smallSide * cos(arc),
                             rect.top() + rect.height() / 2.0 + smallSide * sin(arc));
    arc = (angle - centerAngle) * M_PI / 180.0;
    p2 = QPointF(rect.left() + rect.width() / 2.0 + smallSide * cos(arc),
                 rect.top() + rect.height() / 2.0 + smallSide * sin(arc));
    po << rect.center() << p1 << end << p2;

    QBrush b(g_config->needleColor);
    QPen pen(QColor(Qt::darkGray));
    pen.setWidthF(1.0);
    p.setBrush(b);
    p.setPen(pen);
    p.drawPolygon(po);

    p.setPen(QColor(Qt::white));
}

void QuCircularGaugeBase::drawGauge(const QRectF &rect, int startAngle, int span, QPainter &p)
{
    QPen pen = p.pen();
    pen.setWidthF(g_config->gaugeWidth);
    pen.setCapStyle(Qt::FlatCap);
    double dx = pen.widthF() / 2.0;
    QRectF ra = rect.adjusted(dx, dx, -dx, -dx);
    int aspan = startAngle, bspan = startAngle, cspan = startAngle + span, dspan = startAngle + span;
    bool error_ok = g_config->low_e < g_config->high_e && g_config->low_e >= g_config->min && g_config->high_e <= g_config->max;
    if(d->readError)
        pen.setColor(g_config->readErrorColor);
    else {
        if(error_ok) {
            // draw err color from min to low err to low warn
            mapTo(g_config->low_e, &aspan);
            mapTo(g_config->high_e, &dspan);
            pen.setColor(g_config->errorColor);
            p.setPen(pen);
            p.drawArc(ra, -startAngle * 16, -(aspan-startAngle) * 16); // 0 degrees is at 3 o'clock
            p.drawArc(ra, -dspan * 16, -16 * (startAngle + span - dspan));
        }
        bool warning_ok = g_config->low_w < g_config->high_w && g_config->low_w >= g_config->min && g_config->high_w <= g_config->max;
        if( (warning_ok && error_ok && g_config->low_w > g_config->low_e && g_config->high_w < g_config->high_e) ||
                (warning_ok && !error_ok) ) {
            pen.setColor(g_config->warningColor);
            p.setPen(pen);
            mapTo(g_config->low_w, &bspan);
            mapTo(g_config->high_w, &cspan);
            p.drawArc(ra, -aspan * 16, -16 * (bspan - aspan));
            p.drawArc(ra, -cspan * 16, -16 * (dspan - cspan)); // starts from 3 o'clock, while our startAngle
        }
        else {

        }
        pen.setColor(g_config->normalColor);
    }
    p.setPen(pen);
    p.drawArc(ra, -(qMax(aspan, bspan)) * 16,  -16 * (qMin(cspan, dspan) - qMax(aspan, bspan)));
}

void QuCircularGaugeBase::drawPivot(const QRectF &rect, QPainter &p)
{
    if(g_config->pivotRadius > 0) {
        QPointF c = rect.center();
        double piv_rad = getPivotRadius();
        QRadialGradient rg(c, piv_rad);
        rg.setColorAt(0, QColor(Qt::black));
        rg.setColorAt(1, QColor(Qt::darkGray));
        QBrush br(rg);
        p.setBrush(br);
        QRectF cr(c.x() - piv_rad, c.y() - piv_rad, 2*piv_rad, 2*piv_rad);
        p.drawEllipse(cr);
    }
}

void QuCircularGaugeBase::drawTicks(const QRectF &r, QPainter &p)
{
    QPen pen = p.pen();
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(3.0);
    double radius = r.width() / 2.0;
    QPointF topL = r.topLeft();
    double step = (g_config->max - g_config->min) / (double) ( (g_config->minorTicksCount + 1) * (g_config->ticksCount -1) ) ;
    double val = g_config->min;
    double tickLen;
    double angle = d->startAngle * M_PI / 180.0;
    const double arc = (double) d->spanAngle / (double) (g_config->ticksCount - 1) * M_PI / 180.0;
    for(int i = 0; i < ticksCount(); i++) {
        double a = angle;
        for(int j = 0; (i < g_config->ticksCount -1 && j < g_config->minorTicksCount + 1) || j < 1; j++) {
            j == 0 ? tickLen = g_config->tickLen : tickLen = g_config->tickLen/2.0;

            QPointF p0 = QPointF(topL.x() + r.width() / 2.0 + radius * cos(a), topL.y() + r.height() / 2.0 + radius * sin(a));
            QPointF p1 = QPointF(topL.x() + r.width() / 2.0 + radius * (1.0 - tickLen) * cos(a),
                                 topL.y() + r.height() / 2.0 + radius * (1.0 - tickLen) * sin(a));

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
                    p1 = QPointF(topL.x() + r.width() / 2.0 + (radius - g_config->gaugeWidth)  * cos(a),
                                 topL.y() + r.height() / 2.0 + (radius - g_config->gaugeWidth) * sin(a));
                    p.drawLine(p0, p1);
                }
                else if(!g_config->drawTickCore1 && g_config->drawTickCore2) {
                    p0 = QPointF(topL.x() + r.width() / 2.0 + (radius - g_config->gaugeWidth)  * cos(a),
                                 topL.y() + r.height() / 2.0 + (radius - g_config->gaugeWidth) * sin(a));
                    p.drawLine(p0, p1);
                }
            }

            a += arc / static_cast<double> (g_config->minorTicksCount + 1);
            val += step;
        }
        angle += arc;
        //  val += step;
    }
}

void QuCircularGaugeBase::drawBackground(const QRectF &rect, QPainter &p)
{
    QPointF c = rect.center();
    double rad = rect.width()/2.0;
    QRadialGradient rg(c, rad);
    QColor c0 = g_config->backgroundColor;
    // use current value instead of the value dedicated to the animation
    // so that the background immediately reflects the value range
    QColor c1;
    d->readError ? c1 = g_config->readErrorColor : c1 = valueColor(g_config->value);
    c0.setAlpha(g_config->backgroundColorAlpha);
    c1.setAlpha(g_config->backgroundColorAlpha);
    QRectF cr(c.x() - rad, c.y() - rad, 2*rad, 2*rad);
    //    rg.setColorAt(0, g_config->backgroundColor);
    rg.setColorAt(g_config->backgroundGradientSpread, c0);
    rg.setColorAt(0, c1);
    QBrush br = QBrush(rg);
    p.setBrush(br);
    p.drawEllipse(cr);
}

void QuCircularGaugeBase::drawLabel(const QRectF &rect, QPainter& p)
{
    QPen pen(Qt::black);
    p.setPen(pen);
    QString txt;

    if(d->label.isEmpty()) {
        txt = formatLabel(g_config->value, d->labelValueFormat);
        if(!g_config->unit.isEmpty())
            txt.append(QString(" [%1]").arg(g_config->unit));
    }
    else
        txt = d->label;

    QFont f = p.font();
    // need to recalculate font size ?
    if(d->cache.label_txt.length() != txt.length() || d->cache.textLabelPtSiz < 0) {
        const int wid = qRound(paintArea().width() / 4.5);
        // start with a font with same size as gauge text labels
        f.setPointSizeF(labelFontSize());
        QFontMetrics fm(f);
        //  decrease if either wider than wid or taller than gauge text labels' height
        while(f.pointSizeF() > 3.0 && (fm.width(txt) > wid || f.pointSizeF() > labelFontSize()) ) {
            f.setPointSizeF(f.pointSizeF() - 0.5);
            fm = QFontMetrics(f);
        }
        d->cache.textLabelPtSiz = f.pointSizeF();
    }
    else {
        // text length didn't change: its size may change only slightly
        // reuse previously calculated pixel size
        f.setPointSizeF(d->cache.textLabelPtSiz);
    }
    // save label text into cache
    d->cache.label_txt = txt;

    if(d->labelFontScale > 0)
        f.setPointSizeF(d->cache.textLabelPtSiz * d->labelFontScale);

    p.setFont(f);
    QFontMetrics fm(f);
    int w = fm.width(txt);
    int h = fm.height();
    double radius = rect.width() / 2.0;
    double cx = rect.center().x();
    double cy = rect.center().y();
    QRectF textR;
    if(d->labelPosition == North) {
        textR = QRectF(cx - w/2.0, cy - radius * d->labelDistFromCenter - h, w, h);
    }
    else if(d->labelPosition == South) {
        textR = QRectF(cx - w/2.0, cy + radius * d->labelDistFromCenter, w, h);
    }
    else if(d->labelPosition == East) {
        textR = QRectF(cx + radius * d->labelDistFromCenter, cy - h/2.0, w, h);
    }
    else if(d->labelPosition == West)
        textR = QRectF(cx - radius * d->labelDistFromCenter - w, cy - h/2.0, w, h);
    else {
        double angle, w_off = 0, h_off = 0;
        if(d->labelPosition == NorthEast) { angle = M_PI / 4.0; h_off = -h; }
        else if(d->labelPosition == SouthEast) { angle = -M_PI/4.0; }
        else if(d->labelPosition == NorthWest) { angle = 3/4.0 * M_PI; h_off = -h; w_off = -w; }
        else if(d->labelPosition == SouthWest) { angle = -3/4.0 * M_PI; w_off = -w; }
        textR = QRectF(cx + radius * d->labelDistFromCenter * cos(angle) + w_off, cy - radius * d->labelDistFromCenter * sin(angle) + h_off, w, h);
    }
    QColor gray(70, 70, 80);
    p.setPen(gray);
    p.drawText(textR, Qt::AlignHCenter|Qt::AlignVCenter, txt);
}

void QuCircularGaugeBase::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::FontChange)
        regenerateCache();
    QWidget::changeEvent(e);
}

void QuCircularGaugeBase::resizeEvent(QResizeEvent *re)
{
    QWidget::resizeEvent(re);
    updatePaintArea();   // first. does not need to know font size
    updateLabelsFontSize(); // second. needs to know updated paint area
    d->cache.textLabelPtSiz = -1; // force optional text label font recalc in drawLabel
}

QSize QuCircularGaugeBase::sizeHint() const
{
    return QWidget::sizeHint();
}

QSize QuCircularGaugeBase::minimumSizeHint() const
{
    QFontMetrics fm(font());
    int labwid = fm.width(d->cache.longestLabel);
    int labhei = fm.height();
    const double wfactor = 1.4;
    double w0, h0;
    double a0 = -d->startAngle * M_PI / 180.0;
    double a1 = -(d->startAngle + d->spanAngle) * M_PI / 180.0;
    if(d->cache.labelsDistrib.size() == 4) {
        w0 = labwid * (d->cache.labelsDistrib[1] + d->cache.labelsDistrib[2]);
        h0 = labhei * (d->cache.labelsDistrib[0] + d->cache.labelsDistrib[3]);
    }
    else {
        w0 = labwid * 5;
        h0 = labhei * 5;
    }
    double h = qMax(h0, h0 + h0 * 0.85 * qMax(fabs(sin(a0)), fabs(sin(a1))));
    //    printf("a0 %f a1 %f --> %d %d\n", -d->startAngle, -(d->startAngle + d->spanAngle),
    //           qRound(-d->startAngle) % 360, qRound(-(d->startAngle + d->spanAngle)) % 360);
    //    printf("label hei %d labhei*6 %f, qMax sin %f h = %f\n", labhei, labhei * hfactor, qMax(sin(a0), sin(a1)), h);
    return QSize(labwid * wfactor, qMin(w0, h));
}

void QuCircularGaugeBase::updatePaintArea()
{
    QRect wr = this->rect();
    // size not changed: use old rect
    if(wr == d->cache.oldRect)
        return;

    double r;
    QRectF area;

    /*
     *        |
     *      2 | 1
     *  ------+-------- 0 deg
     *      3 | 4  |
     *        |  <-+
     *
     *       90 deg
     */
    double r_a;
    double margin_w, margin_h;
    if((d->quad == q1 || d->quad == q2 || d->quad == q3 || d->quad == q4) ) {
        margin_h = m_getMarginH(qMin(height(), width()));
        margin_w = m_getMarginW(qMin(height(), width()));
    }
    else {
        margin_h = m_getMarginH(height()/2.0);
        margin_w = m_getMarginW(wr.width()/2.0);
    }

    if(d->spanAngle >= 270) {
        if(wr.width() > wr.height())
            area = QRectF(0, 0, wr.height(), wr.height());
        else
            area =  QRectF(0, 0, wr.width(), wr.width());
    }
    else if(d->quad == q1 || d->quad == q2 || d->quad == q3 || d->quad == q4) {
        // needed height
        double dim, m;
        double modmaxcos = qMax(fabs(d->cos_a0), fabs(d->cos_a1));
        double modmaxsin = qMax(fabs(d->sin_a0), fabs(d->sin_a1));
        m = qMax(modmaxcos, modmaxsin);

        margin_h *= 1.3;
        margin_w *= 1.3;
        //      m = fabs(d->sin_a0);
        dim = height() - margin_h;
        r_a = dim / m;
        if(r_a * modmaxcos +  margin_w > wr.width()) {
            r_a = (wr.width() - margin_w) / modmaxcos;
        }
        if(d->quad == q1) {
            //            printf("Q1\n");
            r_a = (wr.height() - margin_h) / -d->sin_a0;
            if(r_a * d->cos_a1 > wr.width() - margin_w)
                r_a = (wr.width() - margin_w) / d->cos_a1;
            area = QRectF(-r_a + margin_w, 0, 2 * r_a, 2 * r_a);
        }
        else if(d->quad == q2) {
            r_a = (wr.height() - margin_h) / -d->sin_a1;
            if(r_a * -d->cos_a0 > wr.width() - margin_w)
                r_a = (wr.width() - margin_w) / -d->cos_a0;
            area = QRectF(0, -r_a *(1 + d->sin_a1), 2 * r_a, 2 *r_a);
        }
        else if(d->quad == q3) {
            r_a = (wr.height() - margin_h) / d->sin_a0;
            if(r_a * -d->cos_a1 > wr.width() - margin_w)
                r_a = (wr.width() - margin_w) / -d->cos_a1;
            area = QRectF(-r_a * (1+d->cos_a1), -r_a + margin_h, 2 * r_a, 2 *r_a); // cos_a1 < 0
        }
        else {
            r_a = (wr.height() - margin_h) / d->sin_a1;
            if(r_a * d->cos_a0 > wr.width() - margin_w)
                r_a = (wr.width() - margin_w) / d->cos_a0;
            area = QRectF(-r_a + margin_w, -r_a + margin_h, 2 * r_a, 2 * r_a);
        }
    }

    // north or south gauge, 2 quadrants
    else if(d->quad == q21 || d->quad == q43) {
        r_a = wr.height() - margin_h;
        if(width() < r_a * fabs(d->cos_a1) + r_a * fabs(d->cos_a0)  + margin_w) {
            r_a = (width() - margin_w) / (fabs(d->cos_a1) + fabs(d->cos_a0));
        }
        if(d->quad == q21) {
            // align the area left side with the rect left
            area = QRectF(-r_a * (1+d->cos_a0),0, 2 *r_a, 2*r_a);
        }
        else { // quad 4, 3 enum 5
            area = QRectF(-r_a * (1+d->cos_a1), -r_a - r_a*d->sin_a1 + margin_h, 2*r_a, 2*r_a);
        }
    }
    // east or west gauge, 2 quadrants
    else if(d->quad == q14 || d->quad == q32) {
        r_a = wr.width() - margin_w;
        if(wr.height() < r_a * fabs(d->sin_a1) + r_a * fabs(d->sin_a0) + margin_h) {
            r_a = (wr.height() - margin_h) / (fabs(d->sin_a1) + fabs(d->sin_a0));
        }
        if(d->quad == q14)
            area = QRectF(-r_a + margin_w, -r_a * (1+d->sin_a0), 2 *r_a, 2*r_a);
        else
            area = QRectF(0, -r_a * (1+d->sin_a1), 2 *r_a, 2*r_a);
    }
    else if(d->quad == q214) {
        r = (wr.height() - margin_h) / (1 + d->sin_a1);
        // w = r - r * d->cos_a0
        if(r * (1 - d->cos_a0) + margin_w > wr.width())
            r = (wr.width() - margin_w) / ( 1- d->cos_a0);
        area = QRectF(-r * (1+d->cos_a0), 0, 2 * r, 2 * r);
    }
    else if(d->quad == q143) {
        r = (wr.height() - margin_h) / (1 - d->sin_a0); // sin(a0) < 0
        if(r * (1 - d->cos_a1) + margin_w  > wr.width())
            r = (wr.width() - margin_w) / (1 - d->cos_a1);
        area = QRectF(-r * (1+d->cos_a1) + margin_w, -r * (1+d->sin_a0) + margin_h, 2 * r, 2 * r);
    }
    else if(d->quad == q432) { // enum 8
        r = (wr.height() - margin_h) / (1 - d->sin_a1); // sin(a1) < 0
        if(r * (1 + d->cos_a0) + margin_w  > wr.width())
            r = (wr.width() - margin_w)  / (1 + d->cos_a0);
        area = QRectF(0, -r - r*d->sin_a1, 2 * r, 2 * r);
    }
    else if(d->quad == q321) {
        r = (wr.height() - margin_h) / (1 + d->sin_a0);  // sin_a0 > 0, cos_a1 > 0
        if(r * (1 + d->cos_a1) + margin_w  > wr.width())
            r = (wr.width() - margin_w)  / (1 + d->cos_a1);
        area = QRectF(0, 0, 2 * r, 2 * r);
    }
    else if(d->quad == q4321) {  // enum 12. a0 in q4 a1 in q1
        r = (wr.width() - margin_w) / (1 + qMax(d->cos_a0, d->cos_a1));
        if(2 * r  > wr.height() - margin_h)
            r = (wr.height() - margin_h) / 2;
        area = QRectF(0, 0, 2 * r, 2 * r);
    }
    else if(d->quad == q3214) {  // enum 13. a0 in q3 a1 in q4, sin_a0 > 0, sin_a1 > 0
        r = (wr.height() - margin_h) / (1 + qMax(d->sin_a0, d->sin_a1));
        if(2 * r  + margin_w  > wr.width())
            r = (wr.width() - margin_w) / 2;
        area = QRectF(0, 0, 2 * r, 2 * r);
    }


    else if(d->quad == q2143) {  // enum 14. a0 in q2 a1 in q3, cos_a0 < 0, cos_a1 < 0
        r = (wr.width() - margin_w) / (1 + qMax(-d->cos_a0, -d->cos_a1));
        if(2 * r  > wr.height() - margin_h)
            r = (wr.height() - margin_h) / 2;
        area = QRectF(-r * (1-qMax(-d->cos_a0, -d->cos_a1)) + margin_w, 0, 2 * r, 2 * r);
    }
    else if(d->quad == q1432) {  // enum 15. a0 in q1 a1 in q2, sin_a0 < 0, sin_a1 < 0
        r = (wr.height() - margin_h) / (1 + qMax(-d->sin_a0, -d->sin_a1));
        if(2 * r  + margin_w  > wr.width())
            r =( wr.width() - margin_w) / 2;
        area = QRectF(0, -r + r*qMax(-d->sin_a0, -d->sin_a1) + margin_h, 2 * r, 2 * r); // sin_a0<0 sin_a1<0
    }

    //    qDebug() << __FUNCTION__  << "r:" << r_a  << area << "quadrant" << d->quad << "margins: w" << margin_w << "h" << margin_h;

    d->cache.paintArea = QRectF(area.topLeft(), QSize(area.width() * 0.95, area.height() * 0.95));
    d->cache.paintArea = area;
}

