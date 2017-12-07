#include "quplotcommon.h"
#include <QtDebug>
#include <math.h>
#include <cumacros.h>
#include <cudata.h>

#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "culinkstats.h"
#include "cucontext.h"
#include <cumbia.h>

#include <quplot_base.h>

class QuPlotCommonPrivate
{
public:
    QPointF pressed_pt, selected_pt;
    bool  auto_scale, bounds_from_autoconf;
    double default_lbound, default_ubound;
    CuContext *context;
};

QuPlotCommon::QuPlotCommon(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    d = new QuPlotCommonPrivate;
    d->context = new CuContext(cumbia, r_fac);
    d->auto_scale = d->bounds_from_autoconf = true;
}

QuPlotCommon::QuPlotCommon(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
{
    d = new QuPlotCommonPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
    d->auto_scale = d->bounds_from_autoconf = true;
}

QuPlotCommon::~QuPlotCommon()
{
    delete d->context; // deletes readers
    delete d;
}

QColor QuPlotCommon::pick_color(int i)
{
    QList<QColor> cl = QList<QColor>() << QColor(Qt::green) << QColor(Qt::blue) <<
                                          QColor(Qt::yellow) << QColor(Qt::gray) <<
                                          QColor(Qt::cyan) << QColor(Qt::magenta) <<
                                          QColor(Qt::darkRed);
    return cl.at(i % cl.size());
}

QStringList QuPlotCommon::sources() const
{
    QStringList l;
    foreach(CuControlsReaderA *r, d->context->readers())
        l.append(r->source());
    return l;
}

void QuPlotCommon::unsetSources(QuPlotBase *plot)
{
    foreach(CuControlsReaderA *r, d->context->readers())
        plot->removeCurve(r->source());

    d->context->disposeReader();
}

void QuPlotCommon::unsetSource(const QString &src, QuPlotBase *plot)
{
    d->context->disposeReader(src.toStdString());
    plot->removeCurve(src);
}

/* Add a list of sources to the plot
 * For each source in l, addSource is called.
 */
void QuPlotCommon::setSources(const QStringList &l,
                              CuDataListener *data_listener)
{

    QStringList srcs = sources();
    foreach(QString s, l)
    {
        if(!srcs.contains(s)) {
            CuControlsReaderA* r = d->context->add_reader(s.toStdString(), data_listener);
            if(r) r->setSource(s);
        }
    }
}

void QuPlotCommon::addSource(const QString &s, CuDataListener *dl)
{
    CuControlsReaderA* r = d->context->add_reader(s.toStdString(), dl);
    if(r) r->setSource(s);
}

void QuPlotCommon::configure(const QString &curveName, const QPointF &pxy)
{
    Q_UNUSED(curveName);
    Q_UNUSED(pxy);
}

CuContext *QuPlotCommon::getContext() const
{
    return d->context;
}

