#include "quanimation.h"
#include <cumacros.h>
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
    bool installed;
    QColor penColor;
};

/*! \brief the class constructor
 *
 * @param t the type of animation, to be chosen amongst QuAnimation::Type
 *
 * The default animation type is QuAnimation::ShrinkingBottomLine
 *
 * \par Default values
 * \li The duration of the underlying QVariantAnimation is set to 1000 milliseconds
 * \li The start and end values of the  underlying QVariantAnimation  are set to 100.0 and 0.0 respectively.
 *     They cannot be directly changed
 * \li the pen color used to draw lines is set to Qt::gray
 * \li The default animation type is QuAnimation::ShrinkingBottomLine
 * \li The ray length for the type QuAnimation::RayOfLight is set to 0.5, meaning half of the widget height
 *
 * A QuAnimation must be first installed (see installOn) on a QWidget in order to be started/stopped/looped.
 *
 */
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
    d->installed = false;
}

/*! \brief install the animation on the given widget
 *
 * The widget to animate must be installed before the animation can be used. Before the widget is
 * installed, start will not work and currentValue will return 0.0
 */
void QuAnimation::installOn(QWidget *w)
{
    d->installed = true;
    QObject::connect(&d->qv_anim, SIGNAL(valueChanged(QVariant)), w, SLOT(onAnimationValueChanged(QVariant)));
}

QuAnimation::~QuAnimation()
{
    delete d;
}

/*! \brief loops the animation with the given type and duration
 *
 * @param t the QuAnimation::Type
 * @param duration, the duration of the animation, in milliseconds
 *
 * If duration is 0 or negative, the duration previously set on the animation is left
 * untouched
 */
void QuAnimation::loop(Type t, int duration)
{
    d->type = t;
    if(d->qv_anim.state() != QAbstractAnimation::Stopped)
        d->qv_anim.stop();
    d->qv_anim.setLoopCount(-1);
    if(duration > 0)
        d->qv_anim.setDuration(duration);
    d->qv_anim.start();
}

/*! \brief starts the animation
 *
 * If the animation is already running, it is stopped and the started over again
 */
void QuAnimation::start() {
    if(d->installed) {
        if(d->qv_anim.state() != QAbstractAnimation::Stopped ) {
            d->qv_anim.stop();
            if(d->qv_anim.loopCount() < 0)
                d->qv_anim.setLoopCount(1);
        }
        d->qv_anim.start();
    }
}

/*! \brief stops the animation
 */
void QuAnimation::stop()
{
    d->qv_anim.stop();
}

/* \brief returns the color of the pen used to draw lines
 *
 * @return the QColor used to draw the lines
 *
 * \par Default color
 * The default color is Qt::gray
 */
QColor QuAnimation::penColor() const
{
    return d->penColor;
}

/*! \brief sets the color of the pen used to draw lines
 *
 * @see penColor
 */
void QuAnimation::setPenColor(const QColor &c)
{
    d->penColor = c;
}

/*! \brief returns the width of the pen used to draw the lines
 *
 * @see penColor
 *
 * \par Default value
 * The default value is 2.0
 */
double QuAnimation::penWidthF() const
{
    return d->penWidthF;
}

/*! \brief change the pen width used to draw the lines
 *
 * @param w the pen width as a floating point
 */
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

/*! \brief change the animation type
 *
 * @param t a new type of animation among QuAnimation::Type
 */
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

/*! \brief returns the length of the ray of light (for type uAnimation::RayOfLight)
 *
 * The actual length of the ray is the product of the widget height and this value
 *
 * @return a factor that, multiplied by the widget height, gives the length of the
 * animation ray segment in pixels.
 *
 * \par Default value
 * The default value is the height divided by two. (Factor of 0.5)
 */
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
 *
 * \note
 * If the animation has not been installed, 0.0 is returned
 */
double QuAnimation::currentValue() const
{
    if(!d->installed)
        return 0.0;
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
    if(d->type == ShrinkingBottomLine || d->type == ExpandingBottomLine) {
        double h;
        d->penWidthF > 0 ? h = 2*d->penWidthF : h = 2;
        return QRect(widgetRect.left(),  widgetRect.bottom() - h, widgetRect.width(), h);
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
