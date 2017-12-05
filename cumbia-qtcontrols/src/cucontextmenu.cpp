#include "cucontextmenu.h"
#include <QAction>

CuContextMenu::CuContextMenu(QWidget *parent) : QMenu(parent)
{
    QAction *info = new QAction("Link stats...", this);
    connect(info, SIGNAL(triggered(bool)), this, SLOT(onInfoActionTriggered()));
    addAction(info);
}

void CuContextMenu::onInfoActionTriggered()
{
    emit linkStatsTriggered(parentWidget());
}
