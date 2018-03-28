#include "treeitem.h"

TreeItem::TreeItem(QTreeWidget *parent, const QStringList& items)
    : QTreeWidgetItem(parent, items)
{

}

bool TreeItem::operator< (const QTreeWidgetItem &other) const
{
    return this->data(0, Qt::UserRole).toInt() < other.data(0, Qt::UserRole).toInt();
}

