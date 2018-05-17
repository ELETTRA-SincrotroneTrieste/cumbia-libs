#include "writer.h"
#include <qubutton.h>
#include <QLineEdit>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <qulineedit.h>
#include <cumacros.h>

Writer::Writer(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, int dim, const QString &tgt) : QWidget(w)
{
    const int maxeditable = 64;
    QGridLayout *contents_grid_lo = new QGridLayout(this);
    const int m_layoutColumnCount = 10;
    QuButton *b = new QuButton(this, cumbia_pool, fpool);
    b->setText("Write");
    QString target = tgt + "(";
    int layout_row = 0;
    int col = 0;
    int j = 0;
    printf("Writer init dim %d\n", dim);
    QLabel *la = new QLabel(tgt, this);
    contents_grid_lo->addWidget(la, layout_row, col, 1, 1);
    for(j = 0; j < dim && j < maxeditable; j++)
    {
        QuLineEdit *le = new QuLineEdit(this, cumbia_pool, fpool);
        if(dim == 1)
            le->setTarget(tgt);
        le->setObjectName(QString("leInputArgs_%1").arg(j));
        if(j > 0 && j % m_layoutColumnCount == 0)
            layout_row++;
        col = j % m_layoutColumnCount + 1;
        contents_grid_lo->addWidget(le, layout_row, col, 1, 1);
        target += "&" + le->objectName();
        if(j < dim -1)
            target += ",";
    }
    contents_grid_lo->addWidget(b, contents_grid_lo->rowCount() -1, col +1, 1, 1);
    if(dim > maxeditable) {
        QLabel *labe = new QLabel(QString("Number of editable elements has been reduced to %1 (dim x is %2)")
                                  .arg(maxeditable).arg(dim), this);
        contents_grid_lo->addWidget(labe, contents_grid_lo->rowCount() , 0, 1, contents_grid_lo->columnCount());
    }

    target += ")";
    b->setTarget(target);
    b->setToolTip(b->target());
}

Writer::~Writer()
{

}

