#ifndef QUPLOTCONTEXTMENUCOMPONENT_H
#define QUPLOTCONTEXTMENUCOMPONENT_H

#include <quplotcomponent.h>

class QPoint;
class QuWidgetContextMenuStrategyI;

class QuPlotContextMenuComponent : public QuPlotComponent
{
public:
    QuPlotContextMenuComponent();

    // QuPlotComponent interface
public:
    QString name() const;
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);

    void execute(QuPlotBase *plot, QuWidgetContextMenuStrategyI* ctxMenuStrategy, const QPoint& pos);
    void print(QuPlotBase *p);
    void snapshot(QuPlotBase *p);
    void copyImage(QuPlotBase *p);
    void saveData(QuPlotBase *p);
    void configurePlot(QuPlotBase *p);
};

#endif // QUPLOTCONTEXTMENUCOMPONENT_H
