#ifndef QUANIMATION_H
#define QUANIMATION_H

class QuAnimationPrivate;
class QWidget;
class QColor;
class QPaintEvent;

#include <QRect>
#include <QVariantAnimation>

/*! \brief A helper class to draw animations on a widget. Presently, a simple animation
 *         is drawn at the bottom of a given rectangle (supposedly, the widget rect)
 *
 * QuAnimation is a class drawing animations within a rectangle. A QuAnimation has a
 * \li type  (currently, the only supported type is QuAnimation::BottomLine)
 * \li a duration (mapped to QVariantAnimation::duration)
 * \li a start and end value (mapped to the respective QVariantAnimation properties)
 *
 * The underlying QVariantAnimation easing curve is set to <cite> QEasingCurve::Linear</cite>
 * The easing curve can be changed accessing QVariantAnimation through the qAnimation call.
 *
 * \par Bottom line animation type
 * The QuAnimation::BottomLine type draws a line at the botton of the given widget
 * that shrinks and disappears within the QuAnimation::duration time in milliseconds
 *
 * \par Ray of light animation type
 * A ray of light flashes all along the drawing rectangle, clockwise, from the top
 * left corner.
 *
 * QuAnimation has two helper methods which a QWidget can use to be animated:
 *
 * \li clippedRect that calculates the minimum rectangle needed to draw the animation
 * \li draw that draws on the widget's rect according to the QuAnimation::Type
 *
 * QuAnimation::start can be used to start or <cite>stop and restart</cite> the animation
 * The progress of the animation is notified using a <cite>Qt slot</cite> named
 *
 * \code
 * void onAnimationValueChanged(const QVariant&)
 * \endcode
 *
 * that must be implemented within the widget
 *
 * \par Example
 * \code
 *
 QuButton::QuButton(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, const QString &text)
    : QPushButton(w) {
    d->context = new CuContext(cumbia_pool, fpool);
    // ...
    m_animation.installOn(this);  // m_animation is a private member of type QuAnimation
  }

 void QuButton::onAnimationValueChanged(const QVariant &v) {
    // optimize animation paint area with the clipped rect provided by QuAnimation::clippedRect
    update(d->animation.clippedRect(rect()));
 }

 void QuButton::paintEvent(QPaintEvent *pe) {
    QPushButton::paintEvent(pe);
    // m_write_ok stores the success of the last operation
    // performed on button click
    //
    if(m_animation.currentValue() != 0.0) {
        m_animation.draw(this, m_write_ok ? QColor(Qt::green) : QColor(Qt::red));
    }
 }
 *
 * \endcode
 *
 */
class QuAnimation
{
public:
    enum Type { ShrinkingBottomLine, ExpandingBottomLine, RayOfLight, ElasticBottomLine };

    QuAnimation(Type t = ShrinkingBottomLine);

    void installOn(QWidget *w);

    virtual ~QuAnimation();

    void loop(Type t);

    void start();

    void stop();

    QColor penColor() const;

    void setPenColor(const QColor& c);

    double penWidthF() const;

    void setPenWidthF(double w);

    Type type() const;

    void setType(Type t);

    void setRayOfLightLen(double percentOfHei);

    double rayOfLightLen() const;

    int duration() const;

    void setDuration(int ms);

    double currentValue() const;

    QVariantAnimation& qAnimation() const;

    QRect clippedRect(const QRect& widgetRect) const;

    virtual void draw(QWidget *w, const QRect& paintRect);

private:
    QuAnimationPrivate *d;
};

#endif // QUANIMATION_H
