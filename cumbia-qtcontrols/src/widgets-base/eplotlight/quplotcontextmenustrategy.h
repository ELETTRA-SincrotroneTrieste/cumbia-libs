#ifndef QUPLOTCONTEXTMENUSTRATEGY_H
#define QUPLOTCONTEXTMENUSTRATEGY_H

#include <quplotbasecontextmenustrategy.h>

class QuPlotContextMenuStrategyPrivate;

class QuPlotContextMenuStrategy : public QuPlotBaseContextMenuStrategy
{
public:
    QuPlotContextMenuStrategy(CuContext *ctx);

    // QuWidgetContextMenuStrategyI interface
public:
    QMenu *createMenu(QWidget *w);

private:
    QuPlotContextMenuStrategyPrivate *d;
};



#endif // QUPLOTCONTEXTMENUSTRATEGY_H
