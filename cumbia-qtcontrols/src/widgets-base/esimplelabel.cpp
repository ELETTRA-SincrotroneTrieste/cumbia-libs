#include "esimplelabel.h"
#include <cumacros.h>
#include <QPainter>
#include <QPen>
#include <QtDebug>

class ESimpleLabelPrivate
{
public:
    QColor borderColor, backgroundColor;
    double borderWidth;
};

ESimpleLabel::ESimpleLabel(QWidget *parent) : QLabel(parent)
{
    d_ptr = new ESimpleLabelPrivate;
    d_ptr->borderWidth = 1.0;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

ESimpleLabel::ESimpleLabel(const QString& text, QWidget *parent) : QLabel(text, parent)
{
    d_ptr = new ESimpleLabelPrivate;
    d_ptr->borderWidth = 1.0;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

ESimpleLabel::~ESimpleLabel()
{
    delete d_ptr;
}

void ESimpleLabel::setDecoration(const QColor & background, const QColor &border)
{
    if(background.isValid() && d_ptr->backgroundColor != background) {
        d_ptr->backgroundColor = background;
        QPalette p = palette();
        p.setColor(QPalette::Background, background);
        setPalette(p);
    }
    d_ptr->borderColor = border;
}

double ESimpleLabel::borderWidth() const
{
    return d_ptr->borderWidth;
}

void ESimpleLabel::setBorderWidth(double w)
{
    d_ptr->borderWidth = w;
    update();
    updateGeometry();
}

QSize ESimpleLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    s.rwidth() += 8 + d_ptr->borderWidth * 2; // (1 outer gray + 1 white + borderWidth + 1 white + 1 margin) * 2
    s.rheight() += 8 + d_ptr->borderWidth * 2;
    return s;
}

/*! \brief Reimplements QLabel::paintEvent
 *
 * Draws a border on the widget:
 * \li one pixel gray border
 * \li one pixel white border
 * \li borderWidth pixel with the color chosen for the border with the setDecoration method
 * \li one pixel white border
 *
 * \note the background color, set with the setDecoration method, is applied through the
 *      QLabel palette for the background role
 */
void ESimpleLabel::paintEvent(QPaintEvent *pe)
{
    qDebug () << __FUNCTION__ << text();
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

