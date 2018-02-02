#include "quledbase.h"
#include "elettracolors.h"
#include <QPainter>
#include <QRadialGradient>
#include <QtDebug>
#include <math.h>

/** @private */
class QuLedBasePrivate
{
public:

    QColor	ledColor, borderColor;
    int width, height, angleDeg, alphaChannel;
    bool rectangular;
    bool gradientEnabled;
    bool linearGradient;
    bool scaleContents;
    double gradientStart;
    double gradientStop;

    QuLedBasePrivate()
    {
        width = height = 18;
        angleDeg = 0;
        rectangular = false;
        gradientEnabled = true;
        linearGradient = false;
        scaleContents = false;
        gradientStart = 0.10;
        gradientStop = 0.95;
        alphaChannel = 255;
        borderColor = QColor(Qt::gray);
    }
};

QuLedBase::QuLedBase(QWidget *parent) : QWidget(parent)
{
    d_ptr = new QuLedBasePrivate();
    d_ptr->ledColor = QColor(Qt::gray);
}

QuLedBase::~QuLedBase()
{
    delete d_ptr;
}

void QuLedBase::setColor(const QColor &c, bool up)
{
    if(c == d_ptr->ledColor)
        return;

    d_ptr->ledColor = c;
    d_ptr->ledColor.setAlpha(d_ptr->alphaChannel);
    if (up)
        update();
}

QColor QuLedBase::color(){ return d_ptr->ledColor; }

int QuLedBase::angle() { return d_ptr->angleDeg; }

void QuLedBase::paintEvent(QPaintEvent *)
{
    QPainter	painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QMatrix m;
    int ledWidth, ledHeight;
    d_ptr->scaleContents ? ledWidth = width() - 2 : ledWidth = d_ptr->width;
    d_ptr->scaleContents ? ledHeight = height() - 2: ledHeight = d_ptr->height;
    // 	painter.rotate(d_ptr->angleDeg);
    qreal		radius  = .45 * qMin(ledWidth, ledHeight);
    qreal x1, y1;
    x1 = width()/2.0 - ledWidth/2.0;
    y1 = height()/2.0 - ledHeight/2.0;

    QPointF  topLeft(x1, y1);
    QRect rect(x1, y1, ledWidth, ledHeight);
    QPointF center(rect.center());

    if(!d_ptr->linearGradient && d_ptr->gradientEnabled)
    {
        QRadialGradient	gradient(.5 * width(), .5 * height(), radius, .5 * (width() - radius), .5 * (height() - radius));
        // 	  QRadialGradient	gradient(center, qMin(ledWidth, ledHeight));
        gradient.setColorAt(d_ptr->gradientStart, EColor(Elettra::white));
        if (isEnabled())
            gradient.setColorAt(d_ptr->gradientStop, d_ptr->ledColor);
        else
            gradient.setColorAt(d_ptr->gradientStop, EColor(Elettra::gray));
        gradient.setColorAt(1, d_ptr->borderColor);
        painter.setBrush(gradient);
    }
    else if(d_ptr->gradientEnabled && d_ptr->linearGradient)
    {
        QLinearGradient	gradient(rect.topLeft(), rect.bottomRight());
        gradient.setColorAt(d_ptr->gradientStart, EColor(Elettra::white));
        if (isEnabled())
            gradient.setColorAt(d_ptr->gradientStop, d_ptr->ledColor);
        else
            gradient.setColorAt(d_ptr->gradientStop, EColor(Elettra::gray));
        gradient.setColorAt(1, d_ptr->borderColor);
        painter.setBrush(gradient);
    }
    else if(!d_ptr->gradientEnabled)
    {
        QBrush b(d_ptr->ledColor);
        painter.setBrush(b);
    }

    m.translate(center.x(), center.y());
    m.rotate(d_ptr->angleDeg);
    m.translate(-center.x(), -center.y());
    painter.setWorldMatrix(m, true);
    painter.setPen(d_ptr->borderColor);

    if(d_ptr->rectangular)
        painter.drawRect(rect);
    else
        painter.drawEllipse(rect);

}

void QuLedBase::setAlphaChannel(int a)
{
    if(a < 0)
    {
        printf("\e[1;33m*\e[0m alpha channel %d is not valid: it must be an integer between 0 and 255. Setting to 0", a);
        d_ptr->alphaChannel = 0;
    }
    else if(a > 255)
    {
        printf("\e[1;33m*\e[0m alpha channel %d is not valid: it must be an integer between 0 and 255. Setting to 255", a);
        d_ptr->alphaChannel = 255;
    }
    d_ptr->alphaChannel = a;
    d_ptr->ledColor.setAlpha(a);
    update();
}

int QuLedBase::alphaChannel() { return d_ptr->alphaChannel; }

bool QuLedBase::gradientEnabled() { return d_ptr->gradientEnabled; }

void QuLedBase::setLinearGradient(bool l)
{
    d_ptr->linearGradient = l;
    update();
}

double QuLedBase::gradientStop() { return d_ptr->gradientStop; }

void QuLedBase::setGradientEnabled(bool grad)
{
    d_ptr->gradientEnabled = grad;
    update();
}

bool QuLedBase::linearGradient() { return d_ptr->linearGradient; }

void QuLedBase::setAngle(int deg)
{
    d_ptr->angleDeg = deg;
    update();
}

void QuLedBase::setLedWidth(int w)
{
    //  setMinimumWidth(w + 2);
    //  if(maximumWidth() < minimumWidth())
    //  	setMaximumWidth(minimumWidth());
    //  if(w < this->width())
    d_ptr->width = w;
    //  else
    //    d_ptr->width = this->width();
    update();
}

void QuLedBase::setLedHeight(int h)
{
    //  setMinimumHeight(h + 2);
    //   if(maximumHeight() < minimumHeight())
    //   	setMaximumHeight(minimumHeight());
    //  if(h < this->height())
    d_ptr->height = h;
    //  else
    //    d_ptr->height = this->height();
    update();
}

int QuLedBase::ledWidth() { return d_ptr->width; }

int QuLedBase::ledHeight() { return d_ptr->height; }

bool QuLedBase::rectangular() { return d_ptr->rectangular; }

void QuLedBase::setRectangular(bool re)
{
    d_ptr->rectangular = re;
    update();
}

void QuLedBase::setScaleContents(bool s)
{
    d_ptr->scaleContents = s;
    update();
}

void QuLedBase::setBorderColor(const QColor &c)
{
    d_ptr->borderColor = c;
    update();
}

QColor QuLedBase::borderColor() const
{
    return d_ptr->borderColor;
}

void QuLedBase::setGradientStop(double val)
{
    d_ptr->gradientStop = val;
    update();
}

double QuLedBase::gradientStart() { return d_ptr->gradientStart; }

void QuLedBase::setGradientStart(double val)
{
    d_ptr->gradientStart = val;
    update();
}

bool QuLedBase::scaleContents() { return d_ptr->scaleContents; }

QSize QuLedBase::sizeHint() const
{
    return QSize(d_ptr->width, d_ptr->height);
}

QSize QuLedBase::minimumSizeHint() const
{
    return this->sizeHint();
}


