#ifndef QUPLOTCOMMON_H
#define QUPLOTCOMMON_H

#include <QPointF>
#include <QColor>

#include <qwt_plot.h>

class QMouseEvent;
class CuControlsReaderA;
class CuControlsReaderFactoryI;
class QuPlotBase;
class CuDataListener;
class Cumbia;
class QuPlotCommonPrivate;
class CuVariant;
class CuData;
class CuContext;
class CuLinkStats;
class CuControlsFactoryPool;
class CumbiaPool;

/** \brief common object owned by cumbia plots used to link and configure a plot.
 *
 * \ingroup plots
 */
class QuPlotCommon
{
public:
    QuPlotCommon(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuPlotCommon(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuPlotCommon();

    QColor pick_color(int i);

    void setSources(const QStringList &l, CuDataListener *data_listener);

    void addSource(const QString& s, CuDataListener *dl);

    void unsetSource(const QString& src, QuPlotBase *plot);

    //void addSource(const QString& src)

    QStringList sources() const;

    void unsetSources(QuPlotBase *plot);

    void configure(const QString& curveName, const QPointF &pxy);

    CuContext *getContext() const;

private:
    QuPlotCommonPrivate* d;
};


#endif // QUPLOTCOMMON_H
