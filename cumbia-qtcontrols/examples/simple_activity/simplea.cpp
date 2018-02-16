#include "simplea.h"
#include <cumbia.h>
#include <QProgressBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <qthreadseventbridgefactory.h>
#include <cuthreadfactoryimpl.h>
#include "activity.h"
#include <QtDebug>

SimpleA::SimpleA(QWidget *parent) : QWidget(parent)
{
   QVBoxLayout *vlo = new QVBoxLayout(this); // layout in two rows
   QProgressBar *pb = new QProgressBar(this); // progress bar
   pb->setMinimum(0); pb->setMaximum(3); pb->setValue(0);
   vlo->addWidget(pb);
   QTreeWidget *tw = new QTreeWidget(this);
   tw->setHeaderLabels(QStringList() << "message" << "activity thread" << "main thread");
   vlo->addWidget(tw);
   QPushButton *pbu = new QPushButton("start", this);
   connect(pbu, SIGNAL(clicked()), this, SLOT(start()));
   vlo->addWidget(pbu);
   QPushButton *pbclear = new QPushButton("clear", this);
   connect(pbclear, SIGNAL(clicked()), tw, SLOT(clear()));
   vlo->addWidget(pbclear);
   QCheckBox *cb = new QCheckBox("same thread", this);
   cb->setObjectName("cbSameTh");
   cb->setToolTip("if this is checked and \"start\" is clicked while another activity\n"
                  "is running, the next activity is started in the same thread as the\n"
                  "running one. Otherwise, another thread is started");
   cb->setChecked(true);
   vlo->addWidget(cb);

   m_cnt = m_thid = 0;
   m_cumbia = new Cumbia();
}

void SimpleA::start()
{
    m_cnt++;
    CuData th_tok("name", "simplea_thread"); // thread token
    if(!findChild<QCheckBox *>("cbSameTh")->isChecked())
        m_thid = m_cnt;
    th_tok["th_id"] = m_thid;
    findChild<QProgressBar *>()->setValue(0);
    Activity *a = new Activity(CuData("name", "simple_activity"));
    m_cumbia->registerActivity(a, this, th_tok,
                               CuThreadFactoryImpl(),
                               QThreadsEventBridgeFactory());
}

void SimpleA::onProgress(int step, int /* total */, const CuData &data)
{
    QString main_th;
    main_th.sprintf("0x%lx", pthread_self());
    findChild<QProgressBar *>()->setValue(step);
    new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << QString::fromStdString(data["thread"].toString()) << main_th);
}

void SimpleA::onResult(const CuData &data)
{
    QString main_th;
    main_th.sprintf("0x%lx", pthread_self());
    QTreeWidgetItem *tw = new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << QString::fromStdString(data["thread"].toString()) << main_th);
    tw->setBackgroundColor(0, QColor(Qt::green));
}

CuData SimpleA::getToken() const
{
    printf("SimpleA::getToken is called\n");
    return CuData("activity", "simple_activity");
}
