#include "tabwidget.h"

void TabBar::mouseReleaseEvent(QMouseEvent *e)
{
    int ci = currentIndex();
    QIcon curIcon = tabIcon(ci);
    if(!curIcon.isNull())
    {
        setTabIcon(ci, QIcon());
    }
    QTabBar::mouseReleaseEvent(e);
}


TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new TabBar(this));
}



