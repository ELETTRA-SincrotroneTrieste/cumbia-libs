#include "quplotcommon.h"
#include <QtDebug>
#include <math.h>
#include <cumacros.h>
#include <cudata.h>

#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "culinkcontrol.h"
#include <cumbia.h>

#include <quplot_base.h>

class QuPlotCommonPrivate
{
public:
    QPointF pressed_pt, selected_pt;
    bool  auto_scale, bounds_from_autoconf;
    double default_lbound, default_ubound;
};

QuPlotCommon::QuPlotCommon()
{
    d = new QuPlotCommonPrivate;
    d->auto_scale = d->bounds_from_autoconf = true;
}

QuPlotCommon::~QuPlotCommon()
{
    foreach(CuControlsReaderA *r, readers)
        delete r;

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
    foreach(CuControlsReaderA *r, readers)
        l.append(r->source());
    return l;
}

void QuPlotCommon::unsetSources(QuPlotBase *plot)
{
    foreach(CuControlsReaderA *r, readers)
    {
        plot->removeCurve(r->source());
        r->unsetSource();
        delete r;
    }
    readers.clear();
}

void QuPlotCommon::unsetSource(const QString &src, QuPlotBase *plot)
{
    QList<CuControlsReaderA *>::iterator it;
    for(it = readers.begin(); it != readers.end(); ++it)
    {
        if((*it)->source() == src)
        {
            plot->removeCurve(src);
            (*it)->unsetSource();
            delete (*it);
            readers.erase(it);
        }
    }
}

/* Add a list of sources to the plot
 * For each source in l, addSource is called.
 */
void QuPlotCommon::setSources(const QStringList &l,
                              const CuLinkControl *link_ctrl,
                              CuDataListener *data_listener)
{
    foreach(QString s, l)
    {
        if(!sources().contains(s))
            addSource(s, link_ctrl, data_listener);
    }
}

void QuPlotCommon::addSource(const QString &s, const CuLinkControl *link_ctrl, CuDataListener *dl)
{
    CuControlsReaderA* r = link_ctrl->make_reader(s.toStdString(), dl);
    r->setSource(s);
    readers.append(r);
}

void QuPlotCommon::configure(const QString &curveName, const QPointF &pxy)
{
    Q_UNUSED(curveName);
    Q_UNUSED(pxy);
}

void QuPlotCommon::getData(CuData& d_inout) const
{
    if(readers.size())
        readers.first()->getData(d_inout);
}

void QuPlotCommon::sendData(const CuData& data)
{
    foreach(CuControlsReaderA* r, readers)
        r->sendData(data);
}

