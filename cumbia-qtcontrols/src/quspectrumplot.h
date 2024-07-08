#ifndef QUSPECTRUMPLOT_H
#define QUSPECTRUMPLOT_H

#include <cudatalistener.h>
#include "quwidgetinterface.h"
#include "quplot_base.h"
#include "quplotbasecontextmenustrategy.h"
#include <cucontexti.h>

class QuSpectrumPlotPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuData;
class CuContext;
class CuVariant;
class CuControlsReaderFactoryI;

/** \brief Draw a line for each data source over time
 *
 * \ingroup plots
 *
 * QuSpectrumPlot is designed to accept a list of data sources.
 * Each data source is represented by a curve in the plot, which is updated according to
 * the CuData "value" and "timestamp_ms" properties.
 * If CuData "err" boolean is true or the "value" data format is not a scalar, the plot
 * is disabled.
 *
 * \par Plot update strategies.
 * Any implementation-specific data update is delegated to an update <b>strategy</b>
 * through setUpdateStrategy. If no update strategy is set, then the update method is called
 * from within onUpdate and the base operations aforementioned are performed.
 * <em>Strategies</em> must implement QuWidgetUpdateStrategyI interface.
 */
class QuSpectrumPlot : public QuPlotBase, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QStringList sources READ sources WRITE setSources DESIGNABLE false)
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE false)
    Q_PROPERTY(int period READ period WRITE setPeriod DESIGNABLE true)

public:
    QuSpectrumPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);
    QuSpectrumPlot(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    QuSpectrumPlot(QWidget *parent);

    virtual ~QuSpectrumPlot();

    QString source() const;
    QStringList sources() const;

    int period() const;
    CuContext *getContext() const;

public slots:

    void setSource(const QString& s);
    void setSources(const QStringList& l);
    void ctxSwap(CumbiaPool *cp, const CuControlsFactoryPool &fp);

    void addSource(const QString &s);
    void unsetSource(const QString &s);
    void unsetSources();

    void setOptions(const CuData &options);
    void setPeriod(int p);
    void requestLinkStats();

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

private:
    QuSpectrumPlotPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);

    // QuWidgetInterface interface
    void update(const CuData &d);
};

#endif // QUTLABEL_H
