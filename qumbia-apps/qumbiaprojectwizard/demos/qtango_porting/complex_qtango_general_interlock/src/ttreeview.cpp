#include "ttreeview.h"
#include <QHeaderView>

TTreeView::TTreeView(QWidget *parent) :
    QTreeView(parent)
{
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void TTreeView::updateFilter()
{
    filter(mCurrentFilter);
}

void TTreeView::filter(const QString&  text)
{
    mCurrentFilter = text;
    for(int i = 0; i< this->model()->rowCount(); i++)
    {
        if(text.isEmpty())
            setRowHidden(i, QModelIndex(), false);

        setRowHidden(i, QModelIndex(),
                     !(model()->data(model()->index(i, 1)).toString().contains(text, Qt::CaseInsensitive)));
    }
}

