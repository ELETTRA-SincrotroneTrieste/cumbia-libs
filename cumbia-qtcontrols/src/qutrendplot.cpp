#include "qutrendplot.h"
#include "quplotcommon.h"
#include <cumacros.h>
#include <cudata.h>
#include <qustring.h>
#include <QtDebug>
#include <QDateTime>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "cucontext.h"
#include "quwidgetupdatestrategy_i.h"
#include "cucontext.h"
#include "quplotcontextmenustrategy.h"
#include "culinkstats.h"
#include "qutimescaledraw.h"
#include <quplotcurve.h>
#include <qwt_date_scale_engine.h>
#include <qwt_plot_directpainter.h>
#include <qwt_painter.h>
#include <cucontrolsutils.h>

/** @private */
class QuTrendPlotPrivate
{
public:
    bool auto_configure, timeScaleDrawEnabled, dateOnTimeScaleDrawEnabled;
    bool read_ok;
    QuPlotCommon *plot_common;
    QuTimeScaleDraw *timeScaleDraw;
    QwtPlotDirectPainter *directPainter;
    CuControlsUtils u;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuTrendPlot::QuTrendPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
    : QuPlotBase(w)
{
    d = new QuTrendPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia, r_fac);
    m_init();
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
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
    d->dateOnTimeScaleDrawEnabled = false;
    setTimeScaleDrawEnabled(true);
    setUpperBoundExtra(QwtPlot::xBottom, 0.1);
    setUpperBoundExtra(QwtPlot::xTop, 0.1);
    d->directPainter = new QwtPlotDirectPainter( this );
    setContextMenuStrategy(new QuPlotContextMenuStrategy(getContext()));
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

/** \brief Set the source name. Replcace currently configured sources, if any.
 *
 * Connect the plot to the source (or sources) specified.
 * More than one source can be set using a ";" separator.
 *
 * @see setSources
 *
 */
void QuTrendPlot::setSource(const QString &s)
{
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    unsetSources();
    setSources(sl);
}

/** \brief Set a list of sources to display on the plot. Replace currently configured ones, if any.
 *
 * @param l a list of strings with the source names.
 */
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
    d->plot_common->unsetSource(s);
    removeCurve(s);
}

void QuTrendPlot::setPeriod(int p)
{
    d->plot_common->getContext()->setOptions(CuData("period", p));
    d->plot_common->getContext()->sendData(CuData("period", p));
}

void QuTrendPlot::unsetSources()
{
    foreach(CuControlsReaderA *r, getContext()->readers())
        removeCurve(r->source());
    d->plot_common->unsetSources();
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
    bool need_replot = false;
    d->read_ok = !da["err"].toBool();
    const QString &src = QString::fromStdString(da["src"].toString()), &msg = d->u.msg(da);

    // update link statistics
    CuLinkStats *link_s = d->plot_common->getContext()->getLinkStats();
    link_s->addOperation();
    if(!d->read_ok)
        link_s->addError(msg.toStdString());

    // configure triggers replot at the end but should not be too expensive
    // to do it once here at configuration time and once more from appendData
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property")
        configure(da);


    QuPlotCurve *crv = curve(src);
    if(!crv) {
        addCurve(src, crv = new QuPlotCurve(src));
    }

    // set the curve state
    d->read_ok ? crv->setState(QuPlotCurve::Normal) : crv->setState(QuPlotCurve::Invalid);

    double x, y;
    if(da.containsKey("timestamp_ms") && crv) {
        CuVariant ts = da["timestamp_ms"];
        ts.getType() == CuVariant::LongInt ? x = static_cast<qint64>(ts.toLongInt()) : x = ts.toDouble();
    }
    else
        x = crv->size() > 0 ? crv->x(crv->size() - 1) + 1 : 0;

    const CuVariant &v = da["value"];
    if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Scalar)
    {
        v.to(y);
        appendData(src, x, y);
    }
    else if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Vector) {
        if(da.containsKey("time_scale_us")) {
            std::vector <double> timestamps = da["time_scale_us"].toDoubleVector();
            us_to_ms(timestamps);
            insertData(src, timestamps.data(), v.toDoubleP(), v.getSize());
        }
        else {
            double *xvals = new double[v.getSize()];
            for(size_t i = 0; i < v.getSize(); i++)
                xvals[i] = i;
            insertData(src, xvals, v.toDoubleP(), v.getSize());
        }
    }
    else if(!d->read_ok && da.containsKey("timestamp_ms")) {
        crv->size() > 0 ? y = crv->lastValue() : y = yLowerBound();
        crv->setText(static_cast<double>(x), msg);
        appendData(src, x, y);
        need_replot = true;
    }

    if(da.containsKey("notes_time_scale_us") && da.containsKey("notes")) {
        need_replot = true;
        std::vector<std::string> notes;
        std::vector<double> notes_ts = da["notes_time_scale_us"].toDoubleVector();
        us_to_ms(notes_ts);
        notes = da["notes"].toStringVector();
        insertData(src, notes_ts.data(), nullptr, notes_ts.size(), yLowerBound());
        for(size_t i = 0; notes_ts.size() == notes.size() && i < notes_ts.size(); i++) {
            crv->setText(notes_ts[i], QuString(notes[i]));
        }
    }
    if(need_replot)
        replot();

    setToolTip(msg);
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
        d->timeScaleDraw->setDateEnabled(d->dateOnTimeScaleDrawEnabled);
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
    d->dateOnTimeScaleDrawEnabled = en;
    if(d->timeScaleDrawEnabled && d->timeScaleDraw)
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
    bool fullReplot = (updateMarker() || curves().size() > 1);
    fullReplot |= updateScales();
    if(fullReplot) {
        QwtPlot::replot();
    }
    else
    {
        foreach(QwtPlotCurve *c, this->curves())
        {
            QwtSeriesData<QPointF> *data = c->data();
            if(data->size() > 1)
                d->directPainter->drawSeries(c, data->size() - 2, data->size() -1);
        }
    }
    resetZoom();
}

void QuTrendPlot::requestLinkStats()
{
    emit linkStatsRequest(this, this);
}

bool QuTrendPlot::showDateOnTimeAxis() const
{
    return d->timeScaleDraw != NULL && d->timeScaleDraw->dateEnabled();
}

/*! \brief returns the refresh period in milliseconds.
 *
 * @return the refresh period, in milliseconds.
 *
 * \par Note
 * The value is taken from the CuContext::options "period" key.
 *
 * \par Get period: method II
 * An alternative way to get the actual period used by the poller is the following
 *
 * \code
 * CuData d_inout("period", -1);
   int period = d->plot_common->getContext()->getData(d_inout);
 * \endcode
 *
 * This second method requires an initialized CuContext link (i.e. after
 * setSource).
 *
 */
int QuTrendPlot::period() const
{
    const CuData& options = d->plot_common->getContext()->options();
    if(options.containsKey("period"))
        return options["period"].toInt();
    return 1000;
}

CuContext *QuTrendPlot::getContext() const
{
    return d->plot_common->getContext();
}

