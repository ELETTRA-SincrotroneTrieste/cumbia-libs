#ifndef TREEITEM_H
#define TREEITEM_H

#include <QTreeWidgetItem>

class TreeItem : public QTreeWidgetItem
{
public:
    TreeItem(QTreeWidget *parent, const QStringList& items);

    bool operator <(const QTreeWidgetItem &other) const;
signals:

public slots:

};

#endif // TREEITEM_H
