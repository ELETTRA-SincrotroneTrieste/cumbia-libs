#include "quplotcontextmenustrategy.h"
#include "cucontextmenu.h"

class QuPlotContextMenuStrategyPrivate {
public:
    CuContextI *contexti;
};

QuPlotContextMenuStrategy::QuPlotContextMenuStrategy(CuContextI *ctxi)
{
    d = new QuPlotContextMenuStrategyPrivate;
    d->contexti = ctxi;
}

QMenu *QuPlotContextMenuStrategy::createMenu(QWidget *w)
{
    QMenu *m = QuPlotBaseContextMenuStrategy::createMenu(w);
    CuContextMenu *cm = new CuContextMenu(w);
    cm->setTitle("More...");
    cm->prepare(d->contexti);
    m->addMenu(cm);
    return m;
}
