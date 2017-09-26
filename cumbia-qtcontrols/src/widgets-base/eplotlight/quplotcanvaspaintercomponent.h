#ifndef QUPLOTCANVASPAINTERCOMPONENT_H
#define QUPLOTCANVASPAINTERCOMPONENT_H

#include <quplotcomponent.h>
#include <QList>

class CuData;
class QwtPlotCurve;
class QPainter;

class QuPlotCanvasPainterComponent : public QuPlotComponent
{
public:
    QuPlotCanvasPainterComponent();

    // QuPlotComponent interface
public:
    QString name() const;

    void attachToPlot(QuPlotBase *) {}
    void connectToPlot(QuPlotBase *) {}

    void update(QPainter *p, const QuPlotBase *plot,
                const QList<QwtPlotCurve *>curves,
                const CuData &options);
};

#endif // QUPLOTCANVASPAINTERCOMPONENT_H
