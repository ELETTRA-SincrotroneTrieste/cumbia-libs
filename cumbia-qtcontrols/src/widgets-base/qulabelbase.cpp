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
    QColor result_border_color, backgroundColor;
    float borderWidth;
    QMap<long int, QPair<QString, QColor> >enum_d;
    QString format;
    int max_len;
    int margin;
    bool draw_internal_border;
};

QuLabelBase::QuLabelBase(QWidget *parent) : QLabel(parent)
{
    d_ptr = new QuLabelBasePrivate;
    d_ptr->borderWidth = 1.0;
    d_ptr->format = "";
    d_ptr->max_len = -1;
    d_ptr->margin = 6;
    d_ptr->draw_internal_border = true;
    setAlignment(Qt::AlignCenter);
    setAutoFillBackground(true);
}

QuLabelBase::QuLabelBase(const QString& text, QWidget *parent) : QLabel(text, parent)
{
    d_ptr = new QuLabelBasePrivate;
    d_ptr->borderWidth = 1.0;
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
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
    d_ptr->backgroundColor = background;
    if(background.isValid()) {
        QPalette p = palette();
        p.setColor(QPalette::Background, background);
        qDebug() << "QuLabelBase::setBackground " <<this << objectName()  << text()  << " with color " << background;
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
    if(border.isValid() && d_ptr->result_border_color != border) {
        d_ptr->result_border_color = border;
    }
}

/*! \brief changes the colors for the decoration of the label
 *
 * @param background sets the background color
 * @param border sets the border color
 *
 * @see borderWidth
 * @see setBackground
 * @see setDrawInternalBorder
 */
void QuLabelBase::setDecoration(const QColor & background, const QColor &border)
{
    const bool bg_update = background.isValid() && d_ptr->backgroundColor != background;
    const bool border_update = d_ptr->result_border_color != border;
    if(border_update)
        d_ptr->result_border_color = border;
    if(bg_update)
        setBackground(background); // calls setPalette: no need for update
   else if(border_update)
        update();
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

/*! \brief sets the width of the border(s) drawn with the chosen color
 *
 * @param w the desired width of the border
 *
 * By default, two borders are drawn:
 * \li a light gray external border;
 * \li an green interior border if the read is successful, a red one otherwise
 *
 * The interior border drawing can be turned off with setDrawInternalBorder
 *
 * @see setDecoration
 */
void QuLabelBase::setBorderWidth(double w)
{
    d_ptr->borderWidth = w;
    update();
    updateGeometry();
}

void QuLabelBase::setDrawInternalBorder(bool draw) {
    d_ptr->draw_internal_border = draw;
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
 * \par Number formatting
 * If QuLabel::format is not empty, it is used to format the number.
 * The format is forwarded to CuVariant::toString. If the latter method is called with an empty format,
 * a default format is used within snprintf according to the data type in use.
 *
 * See CuVariant::toString for further details.
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

/*! \brief if enabled, an external and an internal border are drawn
 *
 * @return true two borders are drawn
 * @return false only the external border is drawn
 *
 * \par Note
 * If the drawInternalBorder property is false, the external border
 * color will represent the result of the reading (green: successful,
 * red: unsuccessful)
 *
 * @see setDrawInternalBorder
 */
bool QuLabelBase::drawInternalBorder() const
{
    return d_ptr->draw_internal_border;
}

/*! \brief returns the format used to represent a number, or an empty string if
 *         no format is specified
 *
 * @return the format to use for number representation
 *
 * @see setFormat
 *
 * \par Note
 * QuLabelBase::setValue forwards the format to CuVariant::toString conversion method.
 */
QString QuLabelBase::format() const
{
    return d_ptr->format;
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
    d_ptr->result_border_color.isValid() ? bor = d_ptr->result_border_color : bor = QColor(Qt::lightGray);

    QPainter p(this);
    QPen pen(bor);
    pen.setJoinStyle(Qt::MiterJoin);

    const QRectF intBorderR = r.adjusted(2 + pwidth/2.0, 2 + pwidth/2.0, -1-pwidth/2.0, -1-pwidth/2.0);

    if(d_ptr->draw_internal_border) {
        pen.setColor(QColor(Qt::lightGray));
        p.setPen(pen);
        p.drawRect(r);
        pen.setColor(Qt::white);
        pen.setWidthF(pwidth + 2);
        p.setPen(pen);
        // draw result border (internal)
        p.drawRect(r.adjusted(1.0 + pen.widthF()/2.0, 1.0+pen.widthF()/2, -pen.widthF()/2.0, -pen.widthF()/2.0));
        pen.setColor(bor);
        pen.setWidthF(pwidth);
        p.setPen(pen);
        p.drawRect(intBorderR); // draw gray border (external)
    }
    else { // draw only result border color, external
        p.setPen(pen);
        p.drawRect(r.adjusted(1.0 + pen.widthF()/2.0, 1.0+pen.widthF()/2, -pen.widthF()/2.0, -pen.widthF()/2.0));
    }
}

