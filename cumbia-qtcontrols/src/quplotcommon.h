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
class CuLinkControl;

class QuPlotCommon
{
public:
    QuPlotCommon();
    virtual ~QuPlotCommon();
    QColor pick_color(int i);

    void setSources(const QStringList &l,
                    const CuLinkControl *link_ctrl,
                    CuDataListener *data_listener);

    void addSource(const QString& s, const CuLinkControl *link_ctrl, CuDataListener *dl);

    void unsetSource(const QString& src, QuPlotBase *plot);

    //void addSource(const QString& src)

    QStringList sources() const;

    void unsetSources(QuPlotBase *plot);

    void configure(const QString& curveName, const QPointF &pxy);

    QList<CuControlsReaderA *> readers;

    void getData(CuData &d_inout) const;

    void sendData(const CuData& data);

private:
    QuPlotCommonPrivate* d;
};


#endif // QUPLOTCOMMON_H
