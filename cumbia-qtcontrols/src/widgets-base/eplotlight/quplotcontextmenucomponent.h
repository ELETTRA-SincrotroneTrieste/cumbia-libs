#ifndef QUPLOTCONTEXTMENUCOMPONENT_H
#define QUPLOTCONTEXTMENUCOMPONENT_H

#include <quplotcomponent.h>

class QPoint;
class CuContext;
class QMenu;
class QuWidgetContextMenuStrategyI;

/*! \brief A QuPlotComponent managing the context menu for a plot.
 *
 * This class, used by cumbia-qtcontrols plots, provides a context menu with default
 * methods to:
 *
 * \li print the plot
 * \li copy the plot as image
 * \li do a snapshot of the plot
 * \li save the data shown in the graph at that moment
 * \li configure the graph parameters (upper, lower bounds, curve colors, and so on)
 *
 * The plot must have a QuWidgetContextMenuStrategyI installed. In this case, the
 * execute method shows the menu populated according to the menu strategy under the
 * mouse pointer.
 *
 */
class QuPlotContextMenuComponent : public QuPlotComponent
{
public:
    QuPlotContextMenuComponent();

    // QuPlotComponent interface
public:
    QString name() const;
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);

    QMenu *getMenu(QuPlotBase *plot, QuWidgetContextMenuStrategyI*  ctxMenuStrategy) const;
    void execute(QuPlotBase *plot, QuWidgetContextMenuStrategyI* ctxMenuStrategy, const QPoint& pos);
    void print(QuPlotBase *p);
    void snapshot(QuPlotBase *p);
    void copyImage(QuPlotBase *p);
    void saveData(QuPlotBase *p);
    void configurePlot(QuPlotBase *p);
};

#endif // QUPLOTCONTEXTMENUCOMPONENT_H
