#include "cuinfodialog.h"
#include <cudatalistener.h>
#include <QMetaObject>
#include <QMetaMethod>
#include <QtDebug>
#include <cumacros.h>

CuInfoDialog::CuInfoDialog(QWidget *parent) : QDialog(parent)
{
    setModal(true);
}

void CuInfoDialog::onInfoRequested(QWidget *sender)
{
    bool res = sender->metaObject()->invokeMethod(sender, "getLinkStats");

    qDebug() << __FUNCTION__ << "result " << res;
    if(sender->metaObject()->indexOfMethod(sender->metaObject()->normalizedSignature("getLinkStats()")) > -1)
    {
        printf("sender %s has method getLinkStats! good\n ");

        this->show();
    }
    else
    {
        qDebug() << __FUNCTION__ << "no method getLinkStats in " << sender;
        for(int i = sender->metaObject()->methodOffset(); i < sender->metaObject()->methodCount(); i++)
            qDebug() << __FUNCTION__ << sender->metaObject()->method(i).methodSignature();
    }
}
