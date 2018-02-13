#ifndef QUPLOTCANVASPAINTERCOMPONENT_H
#define QUPLOTCANVASPAINTERCOMPONENT_H

#include <quplotcomponent.h>
#include <QList>

class CuData;
class QwtPlotCurve;
class QPainter;

/*! \brief plot canvas painting can be delegated to this object
 *
 * This class is used to decorate the plot and display text on the canvas.
 * QuPlotBase::drawCanvas uses this component.
 */
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
