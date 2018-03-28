#ifndef TTREEVIEW_H
#define TTREEVIEW_H

#include <QTreeView>

class TTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TTreeView(QWidget *parent = 0);
    
signals:
    
public slots:
    void filter(const QString&  text);

    void updateFilter();

private:
    QString mCurrentFilter;
    
};

#endif // TTREEVIEW_H
