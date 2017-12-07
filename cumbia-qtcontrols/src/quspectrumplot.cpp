#include "quspectrumplot.h"
#include "quplotcommon.h"
#include <cumacros.h>
#include <cudata.h>
#include <QtDebug>
#include <QDateTime>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "quwidgetupdatestrategy_i.h"
#include "culinkstats.h"
#include "cucontext.h"
#include "quplotbasecontextmenustrategy.h"

#include "qutimescaledraw.h"
#include <QImage>
#include <quplotcurve.h>
#include <qwt_date_scale_engine.h>

class QuSpectrumPlotPrivate
{
public:
    bool auto_configure, timeScaleDrawEnabled;
    bool read_ok;
    CuContext *context;

    QuPlotCommon *plot_common;
    QuTimeScaleDraw *timeScaleDraw;
    QVector<double> x_data;

    void fill_x_data(int c)
    {
        x_data.clear();
        for(int i = 0; i < c; i++)
            x_data << static_cast<double>(i);
    }
};

QuSpectrumPlot::QuSpectrumPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
    : QuPlotBase(w)
{
    d = new QuSpectrumPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia, r_fac);
    m_init();
}

QuSpectrumPlot::QuSpectrumPlot(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QuPlotBase(w)
{
    d = new QuSpectrumPlotPrivate;
    d->plot_common = new QuPlotCommon(cumbia_pool, fpool);
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
    setContextMenuStrategy(new QuPlotBaseContextMenuStrategy());
}

QString QuSpectrumPlot::source() const
{
    return sources().join(";");
}

QStringList QuSpectrumPlot::sources() const
{
    printf("Askinf for sources to plot common %p size %d\n", d->plot_common, d->plot_common->sources().size());
    return d->plot_common->sources();
}

void QuSpectrumPlot::setSource(const QString &s)
{
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    unsetSources();
    setSources(sl);
}

void QuSpectrumPlot::setSources(const QStringList &l)
{
    unsetSources();
    d->plot_common->setSources(l, this);
}

void QuSpectrumPlot::addSource(const QString &s)
{
    d->plot_common->addSource(s, this);
}

void QuSpectrumPlot::unsetSources()
{
    d->plot_common->unsetSources(this);
}

void QuSpectrumPlot::unsetSource(const QString& src)
{
    d->plot_common->unsetSource(src, this);
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
    d->plot_common->getContext()->sendData(CuData("period", p));
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
    CuData d_inout("period", -1);
    d->plot_common->getContext()->getData(d_inout);
    return d_inout["period"].toInt();
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
    d->read_ok = !da["err"].toBool();
    const CuVariant &v = da["value"];
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

        d->read_ok &= (v.isValid() && v.getFormat() == CuVariant::Vector);
        d->read_ok ? crv->setState(QuPlotCurve::Normal) : crv->setState(QuPlotCurve::Invalid);

        setEnabled(d->read_ok);

        if(d->read_ok)
        {
            if(da.containsKey("timestamp_ms") && crv)
            {

            }
            std::vector<double> out;
            v.toVector<double>(out);
            QVector<double> y = QVector<double>::fromStdVector(out);
            if(y.size() != d->x_data.size())
                d->fill_x_data(y.size());
            setData(src, d->x_data, y);
        }
    }
    setToolTip(da["msg"].toString().c_str());
}

void QuSpectrumPlot::requestLinkStats()
{
    emit linkStatsRequest(this, this);
}

CuContext *QuSpectrumPlot::getContext() const
{
    return d->plot_common->getContext();
}





