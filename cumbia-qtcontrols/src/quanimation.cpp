#include "quanimation.h"
#include <QPainter>
#include <QWidget>
#include <QtDebug>
#include <math.h>

class QuAnimationPrivate {
public:
    QuAnimation::Type type;
    QVariantAnimation qv_anim;
    double penWidthF;
    double rayLen;
    QColor penColor;
};

QuAnimation::QuAnimation(Type t)
{
    d = new QuAnimationPrivate;
    d->qv_anim.setDuration(1000);
    d->qv_anim.setStartValue(100.0);
    d->qv_anim.setEndValue(0.0);
    d->penWidthF = 2.0;
    d->rayLen = 0.5;
    d->type = t;
    d->penColor = QColor(Qt::gray);
}

void QuAnimation::installOn(QWidget *w)
{
    QObject::connect(&d->qv_anim, SIGNAL(valueChanged(QVariant)), w, SLOT(onAnimationValueChanged(QVariant)));
}

QuAnimation::~QuAnimation()
{
    delete d;
}

void QuAnimation::loop(Type t)
{
    d->type = t;
    if(d->qv_anim.state() != QAbstractAnimation::Stopped)
        d->qv_anim.stop();
    d->qv_anim.setLoopCount(-1);
    d->qv_anim.start();
}

/*! \brief starts the animation
 *
 * If the animation is already running, it is stopped and the started over again
 */
void QuAnimation::start()
{
    printf("\e[0;36mstart animation: status %d loopCnt %d/%d value %f\e[0m\n",
           d->qv_anim.state(), d->qv_anim.loopCount(), d->qv_anim.currentLoop(), currentValue());
    if(d->qv_anim.state() != QAbstractAnimation::Stopped && d->qv_anim.loopCount() > 0) {
        printf("\e[1;36mSTOPPING ANIMATION cause its loop cnt is %d\e[0m\n", d->qv_anim.loopCount());
        d->qv_anim.stop();
    }
    else if(d->qv_anim.state() == QAbstractAnimation::Running && d->qv_anim.loopCount() < 0) {
        if(currentValue() < (d->qv_anim.startValue().toDouble() - d->qv_anim.endValue().toDouble())/2.0) {

            d->qv_anim.setLoopCount(2);
            printf("\e[0;36m -i- case 1 setted loop count to %d (cur val %f)\e[0m\n", d->qv_anim.currentLoop(),
                   currentValue());
        }
        else {
            d->qv_anim.setLoopCount(1);
            printf("\e[0;36m -ii- case 2 setted loop count to %d (cur val %f)\e[0m\n", d->qv_anim.loopCount(),
                   currentValue());
        }
    }
    else {
        d->qv_anim.start();
    }
}

/*! \brief stops the animation
 */
void QuAnimation::stop()
{
    d->qv_anim.stop();
}

QColor QuAnimation::penColor() const
{
    return d->penColor;
}

void QuAnimation::setPenColor(const QColor &c)
{
    d->penColor = c;
}

/*! \brief returns the width of the pen used to draw the lines
 *
 */
double QuAnimation::penWidthF() const
{
    return d->penWidthF;
}

void QuAnimation::setPenWidthF(double w) {
    d->penWidthF = w;
}

/*! \brief returns the animation type.
 *
 * \note Currently QuAnimation::BottomLine is the only implemented type.
 *
 * The QuAnimation::BottomLine type draws a line at the botton of the given widget
 * that shrinks and disappears within the QuAnimation::duration time in milliseconds
 */
QuAnimation::Type QuAnimation::type() const
{
    return d->type;
}

void QuAnimation::setType(QuAnimation::Type t)
{
    d->type = t;
}

/*! \brief set the length of the ray of light (if animation type is RayOfLight)
 *         in terms of percentage respect to the drawing rectangle height
 *
 * @param percentOfHei value greater than 0.0 that is multiplied by the drawing
 *        rect height to calculate the length of the ray of light
 * \note
 * This applies to QuAnimation::RayOfLight animation type only
 *
 * \par Default value
 * The default value is 0.5, half of the drawing rect height
 *
 * @see setType
 */
void QuAnimation::setRayOfLightLen(double percentOfHei)
{
    d->rayLen = percentOfHei;
}

double QuAnimation::rayOfLightLen() const
{
    return d->rayLen;
}

/*! \brief returns the duration of the animation, in milliseconds
 *
 * Shortcut for QAbstractAnimation::duration
 */
int QuAnimation::duration() const
{
    return d->qv_anim.duration();
}

/*! \brief sets the duration of the animation
 *
 * Shortcut for QAbstractAnimation::setDuration
 */
void QuAnimation::setDuration(int ms) {
    d->qv_anim.setDuration(ms);
}

/*! \brief returns the current value, as double
 *
 * @return shortcut for \codeline QVariantAnimation::currentValue().toDouble()
 */
double QuAnimation::currentValue() const
{
    return d->qv_anim.currentValue().toDouble();
}

/*! \brief returns a const reference to the underlying QVariantAnimation
 *
 * @return const reference to the QVariantAnimation used by this class
 */
QVariantAnimation &QuAnimation::qAnimation() const
{
    return d->qv_anim;
}

/*! \brief given the widget rectangle, returns the clipped rectangle that optimizes
 *         the paint area.
 *
 * @param widgetRect the rectangle of the widget to be animated
 * @return an optimized rectangle with the minimum area that involves the update
 *         region, according to the chosen QuAnimation::Type
 *
 * \par Example
 * If we are using the QuAnimation::BottomLine animation, a rectangle with
 * the input widgetRect's full width and a height of just a few pixels is returned
 * in order to minimize the paint area.
 * If the animation type is QuAnimation::RayOfLight: then the whole widget rect is
 * returned.
 *
 * \par Usage
 * Within the widget's onAnimationValueChanged SLOT, the returned value of this method can be
 * used as input to the QWidget::update(const QRect&) call
 *
 * \par Example usage
 * QuButton::onAnimationValueChanged slot and QuButton::paintEvent provide a working example.
 */
QRect QuAnimation::clippedRect(const QRect &widgetRect) const
{
    if(d->type == ShrinkingBottomLine) {
        double h;
        d->penWidthF > 0 ? h = 2*d->penWidthF : h = 2;
        return QRect(widgetRect.left(),  widgetRect.bottom() - h, widgetRect.right(), h);
    }
    return widgetRect;
}

/*! \brief draw on the given widget
 *
 * @param w the widget that is object of the animation
 * @param step the current step of the animation, between start and end values
 * @param color the color used to draw the animation
 *
 */
void QuAnimation::draw(QWidget *w, const QRect &paintRect)
{
    const double sv = d->qv_anim.startValue().toDouble();
    const double cv = d->qv_anim.currentValue().toDouble(); // current value
    QPainter p(w);
    QPen pen = p.pen();
    pen.setWidth(d->penWidthF);
    pen.setColor(d->penColor);
    p.setPen(pen);

    if(d->type == ShrinkingBottomLine || d->type == ElasticBottomLine ||
            d->type == ExpandingBottomLine)
    {
        double h;
        d->penWidthF > 0 ? h = d->penWidthF : h = 2;
        double x = paintRect.width()/2.0;
        double x0 = paintRect.left();
        double y = paintRect.bottom() - h/2.0;
        double l;
        if(d->type == ShrinkingBottomLine) {
            l = x * (sv - cv) / sv;
            p.drawLine(x0 + l, y, x0 + 2*x - l, y);
        }
        else if(d->type == ElasticBottomLine) {
            if(cv > sv/2.0) {
                // expand
                l = x * (sv - cv)/(sv/2.0);
                p.drawLine(x0 + x - l, y, x0 + x + l, y);
            }
            else {
                l = x * (sv/2.0 - cv)/(sv/2.0);
                p.drawLine(x0 + l, y, x0 + 2*x - l, y);
            }
        }
    }
    else if(d->type == RayOfLight) {
        // margin
        const double margin = 2.0;
        const QRect r = paintRect.adjusted(margin, margin, -margin, -margin);
        // ray of lite len
        const double len = qMin(r.width(), r.height()) * d->rayLen;
        // perimeter
        double per = 2*r.height() + 2*r.width();
        // position as fraction of perimeter
        double pos = per *(1 - cv / sv);
        // start from top left and draw all around the border
        double x1, y1;
        if(pos <= r.width() - len) { // draw along top
            x1 = pos;
            y1 = r.top();
            p.drawLine(x1, y1, x1 + len, y1);
        }
        else if(pos > r.width() - len && pos < r.width()) {
            // top right corner
            double v = len - (r.width() - pos); // vertical section
            p.drawLine(pos, r.top(), r.right(), r.top());
            p.drawLine(r.right(), r.top(), r.right(), r.top() + v);
        }
        else if(pos >= r.width() && pos <= per/2.0 - len) {
            // draw along right
            x1 = r.right();
            y1 = pos - r.width();
            p.drawLine(x1, y1, x1, y1 + len);
        }
        else if(pos > per/2.0 - len && pos <= per/2.0 ) {
            // bottom rite corner
            double h = len - (per/2.0 - pos); // horizontal section
            p.drawLine(r.right(), r.top() + pos - r.width(), r.right(), r.bottom());
            p.drawLine(r.right(), r.bottom(), r.right() - h, r.bottom());
        }
        else if(pos > per / 2.0 && pos <= per - r.height() - len) {
            // draw along bottom
            x1 = r.right() - (pos - per/2.0);
            y1 = r.bottom();
            p.drawLine(x1, y1, x1 - len, y1);
        }
        else if(pos > per - r.height() - len && pos <= per - r.height()) {
            // bottom left corner
            double partial = per/2.0 + r.width() - pos;
            p.drawLine(r.left(), r.bottom(), r.left() + partial, r.bottom());
            p.drawLine(r.left(), r.bottom(), r.left(), r.bottom() - (len - partial) );
        }
        else {
            // up along left
            x1 = r.left();
            y1 = r.bottom() - (pos - per/2.0 - r.width());
            p.drawLine(x1, y1, x1, qMax(static_cast<double>(r.top()), y1 - len));
        }

    }
}
