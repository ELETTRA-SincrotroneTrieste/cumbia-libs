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
    CuContextMenu *cm = new CuContextMenu(w);
    cm->setTitle("More...");
    cm->prepare(d->context);
    m->addMenu(cm);
    return m;
}
