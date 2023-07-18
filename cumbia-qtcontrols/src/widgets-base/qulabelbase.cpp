#include "qulabelbase.h"
#include <cumacros.h>
#include <cuvariant.h>
#include <QPainter>
#include <QPen>
#include <QtDebug>
#include <QMap>
#include <QPair>


QuLabelBase::QuLabelBase(QWidget *parent) : QLabel(parent)
{
    d_data = new QuLabelBaseData;
    d_data->borderWidth = 1.0;
    d_data->format = "";
    d_data->max_len = -1;
    d_data->size_scale = 1.12;
    d_data->draw_internal_border = true;
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setAutoFillBackground(true);
}

QuLabelBase::QuLabelBase(const QString& text, QWidget *parent) : QLabel(text, parent)
{
    d_data = new QuLabelBaseData;
    d_data->borderWidth = 1.0;
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setAutoFillBackground(true);
}

QuLabelBase::~QuLabelBase()
{
    delete d_data;
}

/*! \brief returns the maximum length of the displayed string
 *
 * Applies to CuVariant::String data type only
 *
 * @see setMaximumLength
 */
int QuLabelBase::maximumLength() const
{
    return d_data->max_len;
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
    d_data->max_len = len;
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
void QuLabelBase::setBackground(const QColor &background) {
    if(d_data->backgroundColor != background && background.isValid()) {
        QPalette p = palette();
        p.setColor(QPalette::Window, background);
        setPalette(p);
        d_data->backgroundColor = background;
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
    if(border.isValid() && d_data->result_border_color != border) {
        d_data->result_border_color = border;
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
    const bool bg_update = background.isValid() && d_data->backgroundColor != background;
    const bool border_update = d_data->result_border_color != border;
    if(border_update)
        d_data->result_border_color = border;
    if(bg_update)
        setBackground(background); // calls setPalette: no need for update
    else if(border_update) {
        update();
    }
}

/*! \brief returns the width, in pixels, of the colored border that is
 *         drawn around the label
 *
 * Please read setBorderWidth documentation for further details
 */
double QuLabelBase::borderWidth() const
{
    return d_data->borderWidth;
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
    d_data->borderWidth = w;
    update();
    updateGeometry();
}

void QuLabelBase::setDrawInternalBorder(bool draw) {
    d_data->draw_internal_border = draw;
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
bool QuLabelBase::decode(const CuVariant& v, QColor &background) const
{
    if(v.getType() == CuVariant::String && v.getFormat() == CuVariant::Scalar) {
        strncpy(d_data->text, v.c_str(), QULABEL_MAXLEN);
    }
    else if(v.getType() == CuVariant::String)
        strncpy(d_data->text, v.toString().c_str(), QULABEL_MAXLEN);
    else if(v.getType() == CuVariant::Boolean) {
        const bool &bv = v.toBool();
        const QVariant& ts = property("trueString"), &fs = property("falseString");
        if(ts.isValid() && fs.isValid())
            strncpy(d_data->text, (bv ? ts.toString().toStdString().c_str() :
                                       fs.toString().toStdString().c_str()), QULABEL_MAXLEN);
        else
            strncpy(d_data->text, v.toString().c_str(), QULABEL_MAXLEN);
        const QVariant& tcp = property("trueColor"), &fcp = property("falseColor");
        if(tcp.isValid() && fcp.isValid()) {
            background = bv ? tcp.value<QColor>() : fcp.value<QColor>();
        }
    }
    else if(v.isInteger() && v.getFormat() == CuVariant::Scalar) {
        long int li;
        v.to<long int>(li);
        if(d_data->enum_d.contains(li)) {
            strncpy(d_data->text, d_data->enum_d[li].first.toStdString().c_str(), QULABEL_MAXLEN);
            background = (d_data->enum_d[li].second);
        }
        else {
            strncpy(d_data->text, v.toString(nullptr, d_data->format.toStdString().c_str()).c_str(), QULABEL_MAXLEN);
        }
    }
    else {
        strncpy(d_data->text, v.toString(nullptr, d_data->format.toStdString().c_str()).c_str(), QULABEL_MAXLEN);
    }
    return true;
}

void QuLabelBase::setEnumDisplay(int val, const QString &text, const QColor &c)
{
    QPair <QString, QColor> p;
    p.first = text;
    p.second = c;
    d_data->enum_d[val] = p;
}

void QuLabelBase::setFormat(const QString &fmt)
{
    d_data->format =  fmt;
}

/*! \brief reimplements QLabel::minimumSizeHint to ensure there is enough space to draw
 *         the decorations on the border
 *
 * @return the minimum size hint for the label
 */
QSize QuLabelBase::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    float extra = (d_data->borderWidth + 2) * 4;
    s = QSize(s.width() + extra, s.height() + extra) * d_data->size_scale;
    return s;
}

QSize QuLabelBase::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    float extra =  (d_data->borderWidth + 2) * 4;
    s=  QSize(s.width() + extra, s.height() + extra) * d_data->size_scale;
    return s;
}

int QuLabelBase::heightForWidth(int w) const
{
    float extra =  (d_data->borderWidth + 2) * 4;
    int h = QLabel::heightForWidth(w + extra);
    h *= d_data->size_scale;
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
    return d_data->draw_internal_border;
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
    return d_data->format;
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

    double pwidth = d_data->borderWidth;
    const QRectF &r = rect().adjusted(0, 0, -1, -1);

    QColor bor;
    d_data->result_border_color.isValid() ? bor = d_data->result_border_color : bor = QColor(Qt::lightGray);

    QPainter p(this);
    QPen pen(bor);
    pen.setJoinStyle(Qt::MiterJoin);

    const QRectF intBorderR = r.adjusted(2 + pwidth/2.0, 2 + pwidth/2.0, -1-pwidth/2.0, -1-pwidth/2.0);

    if(d_data->draw_internal_border) {
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
        pen.setWidthF(d_data->borderWidth);
        p.setPen(pen);
        // p.drawRect(r.adjusted(1.0 + pen.widthF()/2.0, 1.0+pen.widthF()/2, -pen.widthF()/2.0, -pen.widthF()/2.0));
        p.drawRect(rect());
    }
}

