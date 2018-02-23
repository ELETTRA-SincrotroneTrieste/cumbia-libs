#include "create-delete.h"

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumacros.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <qulabel.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <cuserviceprovider.h>
#include <qulogimpl.h>
#include <QtDebug>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <cutango-world.h>
#include <QTimer>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>

#ifdef QUMBIA_EPICS_CONTROLS

#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#include <cuepreadoptions.h>
#include <queplotupdatestrategy.h>
#endif

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

CreateDelete::CreateDelete(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{    
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());

#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());

    QVBoxLayout *vlo = new QVBoxLayout(this);
    QStringList dlist = getDevList();
    if(dlist.count() > 0)
    {
        for(int i = 0; i < dlist.count(); i++)
        {

            {
                QGroupBox *gbreaders = new QGroupBox(dlist.at(i) , this);
                gbreaders->setProperty("arg", QString::number(i));
                gbreaders->setObjectName("gbreaders_" + QString::number(i));
                vlo->addWidget(gbreaders);
                new QGridLayout(gbreaders);
            }
        }
        QGroupBox *gbconf = new QGroupBox("Configuration", this);
        gbconf->setObjectName("gbconfig");
        vlo->addWidget(gbconf);

        QGridLayout *gloc = new QGridLayout(gbconf);

        QLabel *lcreate = new QLabel("Number of readers to create: ", gbconf);
        gloc->addWidget(lcreate, 0, 0, 1, 1);
        QSpinBox *sb = new QSpinBox(gbconf);
        sb->setObjectName("sbCreateCnt");
        sb->setValue(2);
        gloc->addWidget(sb, 0, 1, 1, 1);
        QLabel *lsetunset = new QLabel("Number of subsequent setSource/unsetSource for testing: ", gbconf);
        gloc->addWidget(lsetunset, 0, 2, 1, 1);
        QSpinBox *sbsu = new QSpinBox(gbconf);
        sbsu->setObjectName("sbSetUnsetCnt");
        sbsu->setValue(2);
        gloc->addWidget(sbsu, 0, 3, 1, 1);
        QPushButton *pb = new QPushButton("Add reader", this);
        connect(pb, SIGNAL(clicked()), this, SLOT(addReaders()));
        gloc->addWidget(pb, 0, 4, 1, 1);

        // delete more than one at once
        QLabel *ldelete = new QLabel("Number of readers to delete from bottom (-1 delete all): ", gbconf);
        gloc->addWidget(ldelete, 1, 0, 1, 1);
        QSpinBox *sbdelcnt = new QSpinBox(gbconf);
        sbdelcnt->setObjectName("sbDeleteCnt");
        sbdelcnt->setValue(2);
        gloc->addWidget(sbdelcnt, 1, 1, 1, 1);
        QPushButton *pbmultidel = new QPushButton("Delete Multiple", this);
        connect(pbmultidel, SIGNAL(clicked()), this, SLOT(deleteMulti()));
        gloc->addWidget(pbmultidel,1, 2, 1, 1);

        foreach(QString a, qApp->arguments())
        {
            if(a.contains("--set-unset-cnt="))
                sbsu->setValue(a.remove("--set-unset-cnt=").toInt());
            else if(a.contains("--set-nreaders="))
                sb->setValue(a.remove("--set-nreaders=").toInt());
        }
        addReaders();
    }
    else // usage!
    {
        QLabel *label = new QLabel(this);
        label->setText("Usage: " + qApp->arguments().first() + " attribute name ");
        vlo->addWidget(label);
    }
    resize(QSize(300, 200));
}

CreateDelete::~CreateDelete()
{
    delete m_ctrl_factory_pool.getReadFactory("tango");
    delete m_ctrl_factory_pool.getWriteFactory("tango");
#ifdef QUMBIA_EPICS_CONTROLS
    delete m_ctrl_factory_pool.getReadFactory("epics");
    delete m_ctrl_factory_pool.getWriteFactory("epics");
#endif

}

void CreateDelete::addReaders()
{
    QString s= qApp->arguments().at(1);
    int count = findChild<QSpinBox*>("sbCreateCnt")->value();
    for(int i = 0; i < count; i++)
        addReader();
}

void CreateDelete::addReader()
{
    for(int i = 0; i < getDevList().size(); i++)
    {
        QString s = getDevList().at(i);
        QGroupBox *gb = findChild<QGroupBox *>("gbreaders_" + QString::number(i));
        QGridLayout *glo = qobject_cast<QGridLayout *>(gb->layout());
        int rows = gb->findChildren<QuLabel *>().size();
        QuLabel *label = new QuLabel(gb, cu_pool, m_ctrl_factory_pool);
        label->setObjectName("label_" + QString::number(rows));
        glo->addWidget(label, rows, 0, 1, 2);
        QSpinBox *sbsu = findChild<QSpinBox *>("sbSetUnsetCnt");
        for(int i = 1; i < sbsu->value(); i++)
        {
            printf("\e[1;35m***\e[0m testing setSource / unsetSource n.%d\n", i);
            label->setSource(s);
            label->unsetSource();
            label->unsetSource();
            label->unsetSource();
        }

        label->setSource(s);
        QPushButton *pb  = new QPushButton("Disconnect", gb);
        pb->setObjectName("pb_" + QString::number(rows));
        pb->setToolTip("Disconnect " + label->objectName());
        connect(pb, SIGNAL(clicked()), this, SLOT(disconnectSrc()));
        glo->addWidget(pb, rows, 3, 1, 1);
        QPushButton *pbd  = new QPushButton("Delete", gb);
        pbd->setToolTip("Delete " + label->objectName());
        pbd->setObjectName("pbdel_" + QString::number(rows));
        glo->addWidget(pbd, rows, 4, 1, 1);
        connect(pbd, SIGNAL(clicked()), this, SLOT(deleteReader()));

        foreach(QWidget *w, gb->findChildren<QWidget*>())
            w->setProperty("arg", QString::number(i));
    }

}

void CreateDelete::disconnectSrc()
{
    QString arg = sender()->property("arg").toString();
    QGroupBox *gb = findChild<QGroupBox *>("gbreaders_" + arg);
    QString id = sender()->objectName().remove("pb_");
    QuLabel *l = gb->findChild<QuLabel *>("label_" + id);
    qobject_cast<QPushButton *>(sender())->setDisabled(true);
    l->unsetSource();
    l->unsetSource();
}

void CreateDelete::deleteReader()
{
    QString id = sender()->objectName().remove("pbdel_");
    QString arg = sender()->property("arg").toString();
    deleteReader(id, arg);
}

void CreateDelete::deleteMulti()
{
    QSpinBox *sbdelcnt = findChild<QSpinBox*>("sbDeleteCnt");
    for(int i = 0; i < getDevList().size(); i++)
    {
        QGroupBox *gb = findChild<QGroupBox *>("gbreaders_" + QString::number(i));
        int cnt  = sbdelcnt->value();

        int r = 0;
        while(r < cnt)
        {
            QuLabel *l = gb->findChild<QuLabel *>();
            if(l)
            {
                QString id = l->objectName().remove("label_");
                delete l;
                delete gb->findChild<QPushButton *>("pbdel_" + id);
                delete gb->findChild<QPushButton *>("pb_" + id);
            }
            r++;
        }

    }
}

void CreateDelete::deleteReader(const QString& row, const QString& gb_arg)
{
    qDebug()  << __FUNCTION__ << " deleting row " << row;
    QGroupBox *gb = findChild<QGroupBox *>("gbreaders_" + gb_arg);
    QuLabel *l = gb->findChild<QuLabel *>("label_" + row);
    delete gb->findChild<QPushButton *>("pb_" + row);
    delete gb->findChild<QPushButton *>("pbdel_" + row);
    delete l;
}

QStringList CreateDelete::getDevList() const
{
    QStringList dl;
    for(int i = 1; i < qApp->arguments().count(); i++)
        if(!qApp->arguments().at(i).startsWith("--set"))
            dl << qApp->arguments().at(i);
    return dl;
}
