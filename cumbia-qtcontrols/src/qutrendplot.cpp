#include "qutrendplot.h"
#include "quplotcommon.h"
#include <cumacros.h>
#include <cudata.h>
#include <QtDebug>
#include <QDateTime>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "cucontext.h"
#include "quwidgetupdatestrategy_i.h"
#include "cucontext.h"
#include "culinkstats.h"
#include "qutimescaledraw.h"
#include <quplotcurve.h>
#include <qwt_date_scale_engine.h>
#include <qwt_plot_directpainter.h>
#include <qwt_painter.h>

class QuTrendPlotPrivate
{
public:
    bool auto_configure, timeScaleDrawEnabled;
    bool read_ok;
    QuPlotCommon *plot_common;
    QuTimeScaleDraw *timeScaleDraw;
    QwtPlotDirectPainter *directPainter;
};

QuTrendPlot::QuTrendPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
    : QuPlotBase(w)
{
    d = new QuTrendPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia, r_fac);
    m_init();
}

QuTrendPlot::QuTrendPlot(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QuPlotBase(w)
{
    d = new QuTrendPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia_pool, fpool);
    m_init();
}

QuTrendPlot::~QuTrendPlot()
{
    pdelete("~QuTrendPlot %p", this);
    delete d->plot_common;
    delete d;
}

void QuTrendPlot::m_init()
{
    d->auto_configure = true;
    d->read_ok = false;
    setTimeScaleDrawEnabled(true);
    setUpperBoundExtra(QwtPlot::xBottom, 0.8);
    setUpperBoundExtra(QwtPlot::xTop, 0.2);
    d->directPainter = new QwtPlotDirectPainter( this );

    if ( QwtPainter::isX11GraphicsSystem() )
        canvas()->setAttribute( Qt::WA_PaintOnScreen, true );
}

QString QuTrendPlot::source() const
{
    return sources().join(";");
}

QStringList QuTrendPlot::sources() const
{
    return d->plot_common->sources();
}

void QuTrendPlot::setSource(const QString &s)
{
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    unsetSources();
    setSources(sl);
}

void QuTrendPlot::setSources(const QStringList &l)
{
    unsetSources();
    d->plot_common->setSources(l, this);
}

void QuTrendPlot::addSource(const QString &s)
{
    d->plot_common->addSource(s, this);
}

void QuTrendPlot::unsetSource(const QString &s)
{
    d->plot_common->unsetSource(s, this);
}

void QuTrendPlot::setPeriod(int p)
{
    d->plot_common->getContext()->sendData(CuData("period", p));
}

void QuTrendPlot::unsetSources()
{
    d->plot_common->unsetSources(this); // this: plot common will remove curves
}

void QuTrendPlot::onUpdate(const CuData &da)
{
    QuWidgetUpdateStrategyI *us = updateStrategy();
    if(!us)
        update(da);
    else
        us->update(da, this);
    emit newData(da);
}

void QuTrendPlot::update(const CuData &da)
{
    d->read_ok = !da["err"].toBool();
    QString src = QString::fromStdString(da["src"].toString());

    // update link statistics
    CuLinkStats *link_s = d->plot_common->getContext()->getLinkStats();
    link_s->addOperation();
    if(!d->read_ok)
        link_s->addError(da["msg"].toString());

    if(d->read_ok && d->auto_configure && da["type"].toString() == "property")
    {
        configure(da);
    }
    else
    {
        QuPlotCurve *crv = curve(src);
        if(!crv)
            addCurve(src, crv = new QuPlotCurve(src));

        d->read_ok &= (da.value().getFormat() == CuVariant::Scalar);
        d->read_ok ? crv->setState(QuPlotCurve::Normal) : crv->setState(QuPlotCurve::Invalid);

        setEnabled(d->read_ok);

        if(d->read_ok && da.containsKey("value"))
        {
            double x, y;
            if(da.containsKey("timestamp_ms") && crv)
                x = static_cast<qint64>(da["timestamp_ms"].toLongInt());
            else
                x = crv->size() > 0 ? crv->x(crv->size() - 1) + 1 : 0;

            da["value"].to(y);
            appendData(src, x, y);
        }
    }
    setToolTip(da["msg"].toString().c_str());
}

void QuTrendPlot::setTimeScaleDrawEnabled(bool enable)
{
    d->timeScaleDrawEnabled = enable;
    /* if xAutoscale is enabled, scales are automatically adjusted at first refresh */
    if(!enable)
    {
        setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw());
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
    }
    else
    {
        d->timeScaleDraw = new QuTimeScaleDraw();
        setAxisScaleDraw(QwtPlot::xBottom, d->timeScaleDraw);
        setAxisLabelRotation(QwtPlot::xBottom, -50.0);
        setAxisScaleEngine(QwtPlot::xBottom, new QwtDateScaleEngine(Qt::LocalTime));
        setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );
    }
    /* deleting d->timeScaleDraw produces a crash */
    replot();
}

bool QuTrendPlot::timeScaleDrawEnabled()
{
    return d->timeScaleDrawEnabled;
}

void QuTrendPlot::setShowDateOnTimeAxis(bool en)
{
    if(d->timeScaleDraw)
        d->timeScaleDraw->setDateEnabled(en);
}

/** \brief Updates the marker and the scales. If the scales are not changed and
 *         the marker is not viisble, then the replot is performed incrementally.
 *
 * If a full replot is needed (a marker is visible or a scale has changed), QwtPlot::replot
 * is called. An incremental draw is performed otherwise.
 */
void QuTrendPlot::refresh()
{
    bool fullReplot = updateMarker();
    fullReplot |= updateScales();
    if(fullReplot)
    {
        //printf("\e[1;32;2mQuTrendPlot: need full REPLOT\e[0m\n");
        QwtPlot::replot();
    }
    else
    {
        foreach(QwtPlotCurve *c, this->curves())
        {
            //printf("\e[1;34;2mQuTrendPlot.replot() replotting only incremental\e[0m\n");
            QwtSeriesData<QPointF> *data = c->data();
            if(data->size() > 1)
                d->directPainter->drawSeries(c, data->size() - 2, data->size() -1);
        }
    }
    resetZoom();
}

bool QuTrendPlot::showDateOnTimeAxis() const
{
    return d->timeScaleDraw != NULL && d->timeScaleDraw->dateEnabled();
}

int QuTrendPlot::period() const
{
    CuData d_inout("period", -1);
    d->plot_common->getContext()->getData(d_inout);
    return d_inout["period"].toInt();
}

CuContext *QuTrendPlot::getContext() const
{
    return d->plot_common->getContext();
}

