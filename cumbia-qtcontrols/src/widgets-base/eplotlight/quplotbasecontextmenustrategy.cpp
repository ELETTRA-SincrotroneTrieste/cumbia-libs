#include "quplotbasecontextmenustrategy.h"
#include "cucontext.h"
#include "cucontextmenu.h"
#include <quplot_base.h>

#include <QMenu>

QMenu *QuPlotBaseContextMenuStrategy::createMenu(QWidget *w)
{
    QuPlotBase *plot = qobject_cast<QuPlotBase *>(w);
    QMenu *rClickM = new QMenu("Plot", plot);
    rClickM->addAction("Print", plot, SLOT(print()))->setObjectName("print");
    rClickM->addAction("Snapshot", plot, SLOT(snapshot()))->setObjectName("snapshot");
    rClickM->addAction("Copy image to clipboard", plot, SLOT(copyImage()))->setObjectName("copy");
    rClickM->addAction("Save Data", plot, SLOT(saveData()))->setObjectName("save");
    rClickM->addSeparator();
    rClickM->addAction("Configure", plot, SLOT(configurePlot()))->setObjectName("configure");
    rClickM->addSeparator();

    return rClickM;
}
