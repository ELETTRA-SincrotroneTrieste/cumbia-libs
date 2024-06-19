#include "quspectrumplot.h"
#include "quplotcommon.h"
#include <cumacros.h>
#include <cudata.h>
#include <QtDebug>
#include <QDateTime>

#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "quwidgetupdatestrategy_i.h"
#include "culinkstats.h"
#include "cucontext.h"
#include "quplotcontextmenustrategy.h"
#include "quplotdatabuf.h"
#include "qutimescaledraw.h"
#include <QImage>
#include <cucontrolsutils.h>
#include <quapplication.h>
#include <quplotcurve.h>
#include <qwt_date_scale_engine.h>

/** @private */
class QuSpectrumPlotPrivate
{
public:
    bool auto_configure, timeScaleDrawEnabled;
    bool read_ok;
    QuPlotCommon *plot_common;
    QuTimeScaleDraw *timeScaleDraw;
    CuControlsUtils u;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuSpectrumPlot::QuSpectrumPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
    : QuPlotBase(w)
{
    d = new QuSpectrumPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia, r_fac);
    m_init();
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuSpectrumPlot::QuSpectrumPlot(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QuPlotBase(w)
{
    d = new QuSpectrumPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia_pool, fpool);
    m_init();
}

/*!
 * \brief Classical, single parent-widget constructor. *QuApplication* properly initialized with
 *        cumbia engine objects is compulsory.
 *
 * \param parent widget
 * \par Important note: cumbia engine references are obtained from the QuApplication instance.
 *      For best performance, static cast of QCoreApplication::instance() to QuApplication is
 *      used.
 *
 * \since cumbia 2.1
 */
QuSpectrumPlot::QuSpectrumPlot(QWidget *parent) : QuPlotBase(parent) {
    d = new QuSpectrumPlotPrivate;
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    d->plot_common = new QuPlotCommon(a->cumbiaPool(), *a->fpool());
    m_init();
}

QuSpectrumPlot::~QuSpectrumPlot()
{
    pdelete("~QuSpectrumPlot %p", this);
    delete d->plot_common;
    delete d;
}

void QuSpectrumPlot::m_init()
{
    d->auto_configure = true;
    d->read_ok = false;
    setContextMenuStrategy(new QuPlotContextMenuStrategy(this));
    setXAxisAutoscaleEnabled(false);
}

QString QuSpectrumPlot::source() const
{
    return sources().join(";");
}

QStringList QuSpectrumPlot::sources() const
{
    return d->plot_common->sources();
}

void QuSpectrumPlot::setSource(const QString &s) {
    QStringList sl = s.split(";");
    unsetSources();
    setSources(sl);
}

void QuSpectrumPlot::setSources(const QStringList &l)
{
    unsetSources();
    d->plot_common->setSources(l, this);
}

void QuSpectrumPlot::ctxSwap(CumbiaPool *cp, const CuControlsFactoryPool& fp) {
    const QStringList &l = d->plot_common->sources();
    d->plot_common->unsetSources();
    //    unsetSources();
    d->plot_common->setSources(l, this, new CuContext(cp, fp));
}

void QuSpectrumPlot::addSource(const QString &s)
{
    d->plot_common->addSource(s, this);
}

void QuSpectrumPlot::unsetSources()
{
    foreach(CuControlsReaderA *r, getContext()->readers())
        removeCurve(r->source());
    d->plot_common->unsetSources();
}

void QuSpectrumPlot::unsetSource(const QString& src)
{
    d->plot_common->unsetSource(src);
    removeCurve(src);
}

/** \brief Changes the refresh period on the plot, issuing a setData on every reader
 *  with a CuData containing the "period" property set to p.
 *
 * QuContext options are left unchanged.
 *
 * \note The effect on the refresh period of the sources depends on the CuControlsReaderA
 * implementation. For example, in the context of the Tango control system, you should expect
 * that a reader managed by events is not affected by a period change.
 */
void QuSpectrumPlot::setPeriod(int p)
{
    d->plot_common->getContext()->setOptions(CuData(TTT::Period, p));
    d->plot_common->getContext()->sendData(CuData(TTT::Period, p));
}

/** \brief Get the refresh period of the sources issuing a getData on the first reader in the list.
 *
 * \par Note 1
 * The effective period of the reader is returned, not the "period" property stored in the CuData
 * options.
 *
 * \par Note 2
 * If each source has been configured with different periods, you should call getContext()->getData
 * with a list of CuData as parameter.
 */
int QuSpectrumPlot::period() const
{
    const CuData& options = d->plot_common->getContext()->options();
    if(options.containsKey(TTT::Period))
        return options[TTT::Period].toInt();
    return 1000;
}

void QuSpectrumPlot::setOptions(const CuData &options)
{
    d->plot_common->getContext()->setOptions(options);
}

void QuSpectrumPlot::onUpdate(const CuData &da)
{
    QuWidgetUpdateStrategyI *us = updateStrategy();
    if(!us)
        update(da);
    else
        us->update(da, this);

    emit newData(da);
}

void QuSpectrumPlot::update(const CuData &da)
{
    d->read_ok = !da[TTT::Err].toBool();  // da["err"]
    const CuVariant &v = da[TTT::Value];  // da["value"]
    const QString &src = QString::fromStdString(da[TTT::Src].toString());  // da["src"]
    // update link statistics
    CuLinkStats *link_s = d->plot_common->getContext()->getLinkStats();
    link_s->addOperation();
    if(!d->read_ok) {
        link_s->addError(d->u.msg(da).toStdString());
        setToolTip(d->u.msg(da));
    }

    QuPlotCurve *crv = curve(src);
    if(!crv) {
        addCurve(src, crv = new QuPlotCurve(src));
        crv->setSamples(new QuPlotDataBuf);
    }
    QuPlotDataBuf *dbuf = static_cast<QuPlotDataBuf *>(crv->data());
    d->read_ok ? crv->setState(QuPlotCurve::Normal) : crv->setState(QuPlotCurve::Invalid);
    if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Vector) {
        v.toVector<double>(dbuf->y);
        if(dbuf->x.empty() && dbuf->y.size() > 0) {
            dbuf->init(dbuf->y.size());
            setXLowerBound(dbuf->x0());
            setXUpperBound(dbuf->xN());
        }
        // configure triggers replot at the end but should not be too expensive
        // to do it once here at configuration time and once more from appendData
        if(d->read_ok && d->auto_configure && da[TTT::Type].toString() == std::string("property")) {  // da["type"]
            configure(da); // QuPlotBase configure
            // initialize x axis data and x axis bounds
        } // end configuration section
        refresh();
    }
    else {
        // If !d->read_ok, there's at least one curve with an Invalid state: replot
        replot();
    }
}

void QuSpectrumPlot::requestLinkStats()
{
    emit linkStatsRequest(this, this);
}

CuContext *QuSpectrumPlot::getContext() const {
    return d->plot_common->getContext();
}

