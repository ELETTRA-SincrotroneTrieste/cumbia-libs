#ifndef QUTRENDPLOT_H
#define QUTRENDPLOT_H

#include <cudatalistener.h>
#include "quwidgetinterface.h"
#include "quplot_base.h"
#include <cucontexti.h>

class QuTrendPlotPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuData;
class CuVariant;
class CuControlsReaderFactoryI;
class CuContext;

/** \brief Draw a line for each data source over time
 *
 * \ingroup plots
 *
 * QuTrendPlot is designed to accept a list of data sources.
 * Each data source is represented by a curve in the plot, which is updated according to
 * the CuData "value" and "timestamp_ms" properties.
 * If CuData "err" flag is true, the associated curve will be decorated accordingly.
 *
 * \par Plot update strategies.
 * Any implementation-specific data update is delegated to an update <b>strategy</b>
 * through setUpdateStrategy. If no update strategy is set, then the update method is called
 * from within onUpdate and the base operations aforementioned are performed.
 * <em>Strategies</em> must implement QuWidgetUpdateStrategyI interface.
 *
 * \par Attach notes to specific x values on the plot \since v1.1
 * If data received by the *onUpdate* method contains the following keys
 * - notes_time_scale_ms (scalar or vector of timestamps as double)
 * - notes (string scalar or vector)
 * and the size of the two values is equal, a note is associated to each
 *
 * @see QuPlotBase
 */
class QuTrendPlot : public QuPlotBase, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE false)
    Q_PROPERTY(QStringList sources READ sources WRITE setSources DESIGNABLE false)
    Q_PROPERTY(int period READ period WRITE setPeriod DESIGNABLE true)
    Q_PROPERTY(bool timeScaleDrawEnabled READ timeScaleDrawEnabled WRITE setTimeScaleDrawEnabled DESIGNABLE true)
    Q_PROPERTY(bool showDateOnTimeAxis READ showDateOnTimeAxis WRITE setShowDateOnTimeAxis DESIGNABLE true)

public:
    QuTrendPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuTrendPlot(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuTrendPlot();

    QString source() const;

    QStringList sources() const;

    bool timeScaleDrawEnabled();

    bool showDateOnTimeAxis() const;

    int period() const;

    CuContext *getContext() const;

public slots:

    void setSource(const QString& s);

    void setSources(const QStringList& l);

    void addSource(const QString& s);

    void unsetSource(const QString& s);

    void unsetSources();

    void setPeriod(int p);

    void setTimeScaleDrawEnabled(bool enable);

    void setShowDateOnTimeAxis(bool en);

    void refresh();

    void requestLinkStats();

signals:
    void newData(const CuData&);

    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

private:
    QuTrendPlotPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);

    // QuWidgetInterface interface
    void update(const CuData &d);

};

#endif // QUTLABEL_H
