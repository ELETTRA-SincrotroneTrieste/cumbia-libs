#include "quplotcontextmenustrategy.h"
#include "cucontextmenu.h"

class QuPlotContextMenuStrategyPrivate {
public:
    CuContext *context;
};

QuPlotContextMenuStrategy::QuPlotContextMenuStrategy(CuContext *ctx)
{
    d = new QuPlotContextMenuStrategyPrivate;
    d->context = ctx;
}

QMenu *QuPlotContextMenuStrategy::createMenu(QWidget *w)
{
    QMenu *m = QuPlotBaseContextMenuStrategy::createMenu(w);
    QMenu *cm = new CuContextMenu(w, d->context);
    cm->setTitle("More...");
    m->addMenu(cm);
    return m;
}
