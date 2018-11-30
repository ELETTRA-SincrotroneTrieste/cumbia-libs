#include "qulabelbase.h"
#include <cumacros.h>
#include <cuvariant.h>
#include <QPainter>
#include <QPen>
#include <QtDebug>
#include <QMap>
#include <QPair>

class QuLabelBasePrivate
{
public:
    QColor borderColor, backgroundColor;
    float borderWidth;
    QMap<long int, QPair<QString, QColor> >enum_d;
    QString format;
    int max_len;
    int margin;
};

QuLabelBase::QuLabelBase(QWidget *parent) : QLabel(parent)
{
    d_ptr = new QuLabelBasePrivate;
    d_ptr->borderWidth = 1.0;
    d_ptr->format = "%.2f";
    d_ptr->max_len = -1;
    d_ptr->margin = 6;
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

/*! \brief returns the maximum length of the displayed string
 *
 * Applies to CuVariant::String data type only
 *
 * @see setMaximumLength
 */
int QuLabelBase::maximumLength() const
{
    return d_ptr->max_len;
}

/*! \brief if the data shown is a string, truncate it to the given length
 *
 * @param len the maximum length of the string displayed by the label or -1
 *        to let the string unmodified.
 *
 * If the string is longer than *len*, it is truncated to *len* characters.
 * The full string is appended to the label tooltip.
 *
 * @see maximumLength
 */
void QuLabelBase::setMaximumLength(int len)
{
    d_ptr->max_len = len;
}

/*! \brief set the background color of the label
 *
 * @param background the background color
 *
 * \note
 * The background is applied only if the color is valid.
 * If the new background is equal to the former one, the label's palette is
 * left untouched.
 *
 * @see setDecoration
 */
void QuLabelBase::setBackground(const QColor &background)
{
    if(background.isValid() && d_ptr->backgroundColor != background) {
        d_ptr->backgroundColor = background;
        QPalette p = palette();
        p.setColor(QPalette::Background, background);
        setPalette(p);
    }
}

/*! \brief changes the border color
 *
 * @param border a QColor
 *
 * \note
 * If either border is not valid or the new color is the same as the current one,
 * nothing is done.
 */
void QuLabelBase::setBorderColor(const QColor &border) {
    if(border.isValid() && d_ptr->borderColor != border) {
        d_ptr->borderColor = border;
    }
}

/*! \brief changes the colors for the decoration of the label
 *
 * @param background sets the background color
 * @param border sets the border color
 *
 * @see borderWidth
 * @see setBackground
 */
void QuLabelBase::setDecoration(const QColor & background, const QColor &border)
{
    setBackground(background);
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

/*! \brief set the value to display on the label. Optionally check if the background
 *         color has been modified by the method
 *
 * @param v the value as CuVariant.
 * @param background_modified, if not NULL, it will be set to
 *        \li true if setValue changed the background of the label
 *        \li false if setValue didn't change the background
 *
 * The background of the label can be changed by set value if either
 * \li the type of data is boolean and the "trueColor" and "falseColor" *properties* are set to a valid QColor *or*
 * \li setEnumDisplay was used to associate integer values to a strings and a colors
 *
 */
void QuLabelBase::setValue(const CuVariant& v, bool *background_modified)
{
    bool bg_modified = false;
    if(v.getType() == CuVariant::String) {
        QString txt = QString::fromStdString(v.toString());
        if(d_ptr->max_len > -1 && txt.length() > d_ptr->max_len) {
            setToolTip(toolTip() + "\n\n" + txt);
            txt.truncate(d_ptr->max_len);
        }
        QLabel::setText(txt);
    }
    else if(v.getType() == CuVariant::Boolean) {
        bool bv = v.toBool();
        QString txt = (bv ? property("trueString").toString() : property("falseString").toString());
        if(txt.isEmpty())
            txt = QString::fromStdString(v.toString());
        QColor background = bv ? property("trueColor").value<QColor>() : property("falseColor").value<QColor>();
        bg_modified = background.isValid();
        if(bg_modified) // valid
            setBackground(background);
        QLabel::setText(txt);
    }
    else if(v.isInteger() && v.getFormat() == CuVariant::Scalar) {
        long int li;
        v.to<long int>(li);
        if(d_ptr->enum_d.contains(li)) {
            QLabel::setText(d_ptr->enum_d[li].first);
            setBackground(d_ptr->enum_d[li].second);
            bg_modified = d_ptr->enum_d[li].second.isValid();
        }
        else {
            QLabel::setText(QString::fromStdString(v.toString(NULL, d_ptr->format.toStdString().c_str())));
        }
    }
    else {
        QLabel::setText(QString::fromStdString(v.toString(NULL, d_ptr->format.toStdString().c_str())));
    }
    if(background_modified)
        *background_modified = bg_modified;
}

void QuLabelBase::setEnumDisplay(int val, const QString &text, const QColor &c)
{
    QPair <QString, QColor> p;
    p.first = text;
    p.second = c;
    d_ptr->enum_d[val] = p;
}

void QuLabelBase::setFormat(const QString &fmt)
{
    d_ptr->format = fmt;
}

/*! \brief reimplements QLabel::minimumSizeHint to ensure there is enough space to draw
 *         the decorations on the border
 *
 * @return the minimum size hint for the label
 */
QSize QuLabelBase::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    float extra = d_ptr->margin  + d_ptr->borderWidth * 2;
    s = QSize(s.width() + extra, s.height() + extra);
    return s;
}

QSize QuLabelBase::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    float extra = d_ptr->margin  + d_ptr->borderWidth * 2;
    s=  QSize(s.width() + extra, s.height() + extra);
    return s;
}

int QuLabelBase::heightForWidth(int w) const
{
    float extra = d_ptr->margin  + d_ptr->borderWidth * 2;
    int h = QLabel::heightForWidth(w + extra);
    h += extra;
    return h;
}

bool QuLabelBase::hasHeightForWidth() const
{
    return QLabel::hasHeightForWidth();
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

