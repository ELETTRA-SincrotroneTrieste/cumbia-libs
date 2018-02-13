#ifndef QUPLOTBASECONTEXTMENUSTRATEGY_H
#define QUPLOTBASECONTEXTMENUSTRATEGY_H

#include <quwidgetcontextmenustrategyi.h>

/**
 * \ingroup plots
 *
 * \brief a context menu strategy to create a context menu for QuPlotBase (strategy pattern)
 *
 * qumbia-tango-controls QuTPlotContextMenuStrategy class is an example of stragegy
 * that inherits from QuPlotBaseContextMenuStrategy.
 *
 * The QuPlotBaseContextMenuStrategy default strategy exposes the functions listed in
 * QuPlotContextMenuComponent documentation.
*/
class QuPlotBaseContextMenuStrategy : public QuWidgetContextMenuStrategyI
{
    // QuWidgetContextMenuStrategyI interface
public:
    QMenu *createMenu(QWidget *w);
};

#endif // QUPLOTBASECONTEXTMENUSTRATEGY_H
