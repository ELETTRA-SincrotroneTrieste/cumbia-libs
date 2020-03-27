#include "quplotcurve.h"
#include <QtDebug>
#include <cumacros.h>
#include <QVariant>
#include <qwt_plot.h>
#include <qwt_scale_div.h>
#include <qwt_painter.h>
#include <QPainter>
#include <elettracolors.h>
#include <qwt_plot_canvas.h>
#include <QPolygon>
#include <qwt_symbol.h>

#define MAX_STATE_SYMBOLS 6
#define STATE_SYMBOL_SIZE 10
#define YLABEL_OFFSET 3

class EPlotCurvePrivate
{
public:
    int bufSize; /* no more needed: managed by EPlotLight */
    CurveData *data;
    QuPlotCurve::State state;
    int max_data_size_for_symbol;
    // keep a boolean to test if symbol is enabled instead of making a call to
    // QwtPlotCurve::symbol
    bool symbol;
    QMap<double, QString> txtmap;
};

QuPlotCurve::QuPlotCurve() :  QwtPlotCurve()
{
    init();
}

QuPlotCurve:: QuPlotCurve(const QwtText &title) : QwtPlotCurve(title)
{
    init();
}

QuPlotCurve:: QuPlotCurve(const QString &title) : QwtPlotCurve(title)
{
    init();
}

void QuPlotCurve::init()
{
    d = new EPlotCurvePrivate;
    d->data = NULL;
    d->data = new CurveData();
    d->bufSize = -1;
    d->state = Normal;
    d->symbol = false;
    d->max_data_size_for_symbol = 5;
}

void QuPlotCurve::m_setSymbol(size_t dataSiz)
{
    QwtSymbol *sym = NULL;
    if(dataSiz <= d->max_data_size_for_symbol) {
        if(!d->symbol) {
            QPen pe(pen());
            pe.setWidthF(1.45);
            sym = new QwtSymbol(QwtSymbol::XCross);
            sym->setPen(pe);
            sym->setSize(10);
            setSymbol(sym);
            d->symbol = true;
        }
    }
    else  if(d->symbol) {
        setSymbol(NULL);
        d->symbol = false;
    }
}

QuPlotCurve::~QuPlotCurve()
{
    if(d->data)
        delete d->data;
    delete d;
}

void QuPlotCurve::setData(const QVector< double > &xData, const QVector< double > &yData)
{
    m_setSymbol(xData.size());
    //    d->data->set(xData, yData);
    QwtPlotCurve::setSamples(xData, yData);
}

void QuPlotCurve::appendData(double *x, double *y, int count)
{
    /* add count elements */
    m_setSymbol(data()->size());
    d->data->append(x, y, count);
}

void QuPlotCurve::insertData(double *x, double *y, int count, double default_y)
{
    /* add count elements */
    m_setSymbol(data()->size());
    d->data->insert(x, y, count, default_y);
}

void QuPlotCurve::popFront()
{
    if(d->data->count() > 0)
        d->data->removeFirstElements(1);
}

void QuPlotCurve::updateRawData()
{
    setRawSamples(d->data->x(), d->data->y(), d->data->count());
}

void QuPlotCurve::clearData()
{
    d->data->removeFirstElements(d->data->count());
    QwtPlotCurve::setSamples(QVector<double>(), QVector<double>());
}

int QuPlotCurve::count() const { return d->data->count(); }

QuPlotCurve::State QuPlotCurve::state() const { return d->state; }

void QuPlotCurve::setState(QuPlotCurve::State s) { d->state = s; }

QString QuPlotCurve::text(const double &x) const {
    QString s;
    if(d->txtmap.contains(x))
        s = d->txtmap[x];
    return s;
}

void QuPlotCurve::setText(const double &x, const QString &txt) {
    d->txtmap[x] = txt;
}

/** \brief one of the drawing methods.
  *
  * This method is invoked to draw the curve from the point from to the point to.
  * When data is appended, the method is normally called with from and to reflecting the distance
  * between the point added and the previous point.
  * When from == 0 and to == -1, then a full redraw is requested.
  * When the state of the curve is Invalid, a certain number of markers, according to the value
  * of the defined MAX_STATE_SYMBOLS, is drawn. In the case of the invalid state, all the symbols
  * are redrawn on the full curve, whether a full draw is requested or not.
  * On the other side, when d->showYValuesEnabled is true, only the requested labels are drawn,
  * according to the value of from and to, eventually all the labels in case of from == 0 and to == -1.
  */
void QuPlotCurve::drawCurve(QPainter *p, int style,
                            const QwtScaleMap &xMap,
                            const QwtScaleMap &yMap,
                            const QRectF &canvasRect,
                            int from, int to) const
{
    double y = 0.0, x = 0.0;
    const QPen& pe = QwtPlotCurve::pen();
    p->setPen(pe);
    const QColor& c = pe.color();
    if(d->state == Invalid)
    {
        /* always (re)draw all the markers, because the number of them is limited to
         * MAX_STATE_SYMBOLS.
         */
        int step;
        QColor errColor(KDARKRED);
        errColor.setAlpha(160);
        QPen errPen(errColor);
        QFont f = p->font();
        f.setPixelSize(0.8  * STATE_SYMBOL_SIZE);
        p->save();
        p->setFont(f);

        /* draw an error message */
        QString errMsg = "Invalid data or read error on one (or more) curves.";
        QPointF bottomRight = canvasRect.bottomRight();
        QFontMetrics fm(f);
        QRect errMsgRect(QPoint(bottomRight.x() - fm.width(errMsg),
                                bottomRight.y() - 2 * fm.height()), QSize(fm.width(errMsg), fm.height()));
        p->setPen(errPen);
        QwtPainter::fillRect(p, errMsgRect, Qt::white);
        QwtPainter::drawText(p, errMsgRect, Qt::AlignHCenter|Qt::AlignCenter, errMsg);
        QString errMsg2 = "Right click and select \"Info...\" for details.";
        errMsgRect.moveTop(errMsgRect.top() + fm.height());
        errMsgRect.setWidth(fm.width(errMsg2));
        QwtPainter::fillRect(p, errMsgRect, Qt::white);
        QwtPainter::drawText(p, errMsgRect, Qt::AlignHCenter|Qt::AlignCenter, errMsg2);

        if(count() / MAX_STATE_SYMBOLS > 0)
            step = count() / MAX_STATE_SYMBOLS;
        else
            step = 1;
        int i = 1;
        while(i * step < count())
        {
            x = data()->sample(i * step).x();
            y = data()->sample(i * step).y();

            i++;
        }
        p->restore();
    }

    QwtPlotCurve::drawCurve(p, style, xMap, yMap, canvasRect, from, to);

    p->setRenderHint(QPainter::Antialiasing, true);
    foreach(double x, d->txtmap.keys()) {
       for(size_t i = 0; i < data()->size(); i++) {
           if(d->data->x()[i] == x) {
               y = d->data->y()[i];
               double x0t, xt = xMap.transform(x);
               double y0t, yt = yMap.transform(y);
               if(i > 0) {
                   x0t = xMap.transform(d->data->x()[i-1]);
                   y0t = yMap.transform(d->data->y()[i-1]);
               }
               QRect rect(xt - STATE_SYMBOL_SIZE / 2, yt- STATE_SYMBOL_SIZE / 2,
                          STATE_SYMBOL_SIZE, STATE_SYMBOL_SIZE);
               QPen ep(Qt::red);
               ep.setWidthF(1.5);
               p->setPen(ep);
               QwtPainter::drawEllipse(p, rect);
               if(i > 0)
                    QwtPainter::drawLine(p, x0t, y0t, xt, yt);
               ep.setColor(Qt::black);
               ep.setWidthF(0.0f);
               p->setPen(ep);
               rect.translate(-rect.width()/1.2f, -rect.height()/1.7f);
               QwtPainter::drawText(p, rect, Qt::AlignHCenter|Qt::AlignCenter, "i");
           }
       }
    }
}

double QuPlotCurve::x(int index) const
{
    return this->data()->sample(index).x();
}

double QuPlotCurve::y(int index) const
{
    return this->data()->sample(index).y();
}

size_t QuPlotCurve::size() const
{
    return this->data()->size();
}

double QuPlotCurve::lastValue() const {
    double v;
    d->data->count() > 0 ? v = d->data->y()[d->data->count() - 1] : v = 0.0;
    return v;
}



