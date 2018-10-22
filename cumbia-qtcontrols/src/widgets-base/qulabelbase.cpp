#include "qulabelbase.h"
#include <cumacros.h>
#include <QPainter>
#include <QPen>
#include <QtDebug>

class QuLabelBasePrivate
{
public:
    QColor borderColor, backgroundColor;
    double borderWidth;
};

QuLabelBase::QuLabelBase(QWidget *parent) : QLabel(parent)
{
    d_ptr = new QuLabelBasePrivate;
    d_ptr->borderWidth = 1.0;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

QuLabelBase::QuLabelBase(const QString& text, QWidget *parent) : QLabel(text, parent)
{
    d_ptr = new QuLabelBasePrivate;
    d_ptr->borderWidth = 1.0;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

QuLabelBase::~QuLabelBase()
{
    delete d_ptr;
}

/*! \brief changes the colors for the decoration of the label
 *
 * @param background sets the background color
 * @param border sets the border color
 *
 * @see borderWidth
 */
void QuLabelBase::setDecoration(const QColor & background, const QColor &border)
{
    if(background.isValid() && d_ptr->backgroundColor != background) {
        d_ptr->backgroundColor = background;
        QPalette p = palette();
        p.setColor(QPalette::Background, background);
        setPalette(p);
    }
    d_ptr->borderColor = border;
}

/*! \brief returns the width, in pixels, of the colored border that is
 *         drawn around the label
 *
 * Please read setBorderWidth documentation for further details
 */
double QuLabelBase::borderWidth() const
{
    return d_ptr->borderWidth;
}

/*! \brief sets the width of the border drawn with the chosen color
 *
 * @param w the desired width of the border
 *
 * The border color is chosen with setDecoration
 *
 * Additional
 * \li one pixel gray outer border
 * \li two one-pixel borders around the colored border
 * are drawn
 *
 * @see setDecoration
 */
void QuLabelBase::setBorderWidth(double w)
{
    d_ptr->borderWidth = w;
    update();
    updateGeometry();
}

/*! \brief reimplements QLabel::minimumSizeHint to ensure there is enough space to draw
 *         the decorations on the border
 *
 * @return the minimum size hint for the label
 */
QSize QuLabelBase::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    s.rwidth() += 8 + d_ptr->borderWidth * 2; // (1 outer gray + 1 white + borderWidth + 1 white + 1 margin) * 2
    s.rheight() += 8 + d_ptr->borderWidth * 2;
    return s;
}

/*! \brief Reimplements QLabel::paintEvent
 *
 * Draws a decoration on the widget:
 * \li one pixel gray outer border
 * \li one pixel white border
 * \li borderWidth pixel with the color chosen for the border with the setDecoration method
 * \li one pixel inner white border
 *
 * \note the background color, set with the setDecoration method, is applied through the
 *      QLabel palette for the background role
 */
void QuLabelBase::paintEvent(QPaintEvent *pe)
{
    QLabel::paintEvent(pe);
    double pwidth = d_ptr->borderWidth;
    const QRectF &r = rect().adjusted(0, 0, -1, -1);
    QColor bor;
    QPainter p(this);
    QPen pen(bor);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(QColor(Qt::gray));
    p.setPen(pen);
    p.drawRect(r);
    pen.setColor(Qt::white);
    pen.setWidthF(pwidth + 2);
    p.setPen(pen);
    p.drawRect(r.adjusted(1.0 + pen.widthF()/2.0, 1.0+pen.widthF()/2, -pen.widthF()/2.0, -pen.widthF()/2.0));
    d_ptr->borderColor.isValid() ? bor = d_ptr->borderColor : bor = QColor(Qt::lightGray);
    pen.setColor(bor);
    pen.setWidthF(pwidth);
    p.setPen(pen);
    p.drawRect(r.adjusted(2 + pwidth/2.0, 2 + pwidth/2.0, -1-pwidth/2.0, -1-pwidth/2.0));
}

