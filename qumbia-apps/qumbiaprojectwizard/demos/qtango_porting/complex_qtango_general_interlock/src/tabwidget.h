#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QTabBar>

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    TabBar(QWidget *parent ) : QTabBar(parent) {}

protected:
    void mouseReleaseEvent(QMouseEvent *e);

};

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    
signals:
    
public slots:

    
};

#endif // TABICON_H
