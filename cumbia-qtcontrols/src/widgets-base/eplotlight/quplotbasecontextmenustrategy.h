#ifndef QUPLOTBASECONTEXTMENUSTRATEGY_H
#define QUPLOTBASECONTEXTMENUSTRATEGY_H

#include <quwidgetcontextmenustrategyi.h>

class QuPlotBaseContextMenuStrategy : public QuWidgetContextMenuStrategyI
{
    // QuWidgetContextMenuStrategyI interface
public:
    QMenu *createMenu(QWidget *w);
};

#endif // QUPLOTBASECONTEXTMENUSTRATEGY_H
