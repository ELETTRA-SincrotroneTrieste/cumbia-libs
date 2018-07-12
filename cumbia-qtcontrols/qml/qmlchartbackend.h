#ifndef QMLCHARTBACKEND_H
#define QMLCHARTBACKEND_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>

#include <QDateTime>
#include <QtCharts/QAbstractSeries>

class QmlChartBackendPrivate;
class CumbiaPool_O;

class QmlChartBackend : public QObject, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString sources READ sources WRITE setSources NOTIFY sourcesChanged)
    Q_PROPERTY(bool readOk READ readOk NOTIFY readOkChanged)
    Q_PROPERTY(bool xAutoscale READ xAutoscale WRITE setXAutoscale NOTIFY xAutoscaleChanged)
    Q_PROPERTY(bool yAutoscale READ yAutoscale WRITE setYAutoscale NOTIFY yAutoscaleChanged)
    Q_PROPERTY(qreal xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(qreal yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(qreal xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(qreal yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)
    Q_PROPERTY(QDateTime t1 READ t1  NOTIFY t1Changed)
    Q_PROPERTY(QDateTime t2 READ t2  NOTIFY t2Changed)
    Q_PROPERTY(int bufsiz READ bufsiz WRITE setBufsiz NOTIFY bufsizChanged)
    Q_PROPERTY(bool scalarTrend READ scalarTrend NOTIFY scalarTrendChanged)

public:
    explicit QmlChartBackend(QObject *parent = nullptr);

    Q_INVOKABLE void init(CumbiaPool_O *poo_o);

    QString sources() const;

    bool readOk() const;

    qreal xMin() const;
    qreal xMax() const;
    qreal yMin() const;
    qreal yMax() const;

    bool timeScale() const;
    bool scalarTrend() const;
    bool xAutoscale() const;
    bool yAutoscale() const;

    int bufsiz() const;

    QDateTime t1() const;
    QDateTime t2() const;

    // CuContextI interface
    virtual CuContext *getContext() const;

    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

signals:
    void sourcesChanged(const QStringList& srcs);

    void readOkChanged();
    void xMinChanged();
    void yMinChanged();
    void xMaxChanged();
    void yMaxChanged();
    void scalarTrendChanged(bool isTrend);
    void bufsizChanged();
    void xAutoscaleChanged();
    void yAutoscaleChanged();

    void newPoint(const QString& src);
    void newSpectrum(const QString& src);


    void t1Changed();
    void t2Changed();


public slots:

    void setSources(const QString &sl);
    void unsetSources();

    void replaceData(QtCharts::QAbstractSeries *series);
    void appendPoint(QtCharts::QAbstractSeries *series);

    void setXMin(qreal xm);
    void setXMax(qreal xM);
    void setYMin(qreal ym);
    void setYMax(qreal yM);

    void setXAutoscale(bool a);
    void setYAutoscale(bool a);

    void setBufsiz(int b);

private:
    QmlChartBackendPrivate *d;

    void m_configure(const CuData& da);

    void m_setSources(const QStringList& srcs);

    QStringList m_sources() const;

    void m_update_curve_y_bounds(const QString& src, const QVector<QPointF> &yv);

    void m_update_curve_y_bounds(const QString& src, const double &y);

    bool m_find_absolute_bounds_and_notify();

};

#endif // TRENDCHARTBACKEND_H
