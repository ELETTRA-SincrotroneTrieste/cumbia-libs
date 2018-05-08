#include "simplea.h"
#include <cumbia.h>
#include <QProgressBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QHeaderView>
#include <qthreadseventbridgefactory.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadservice.h>
#include <cuserviceprovider.h>
#include "activity.h"

SimpleA::SimpleA(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vlo = new QVBoxLayout(this); // layout in two rows
    QProgressBar *pb = new QProgressBar(this); // progress bar
    pb->setMinimum(0); pb->setMaximum(3); pb->setValue(0);
    vlo->addWidget(pb);
    QTreeWidget *tw = new QTreeWidget(this);
    tw->setHeaderLabels(QStringList() << "activity" << "message" << "activity thread" << "main thread");
    tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
    resize(500, 400);

    m_cnt = m_thid = 0;
    m_cumbia = new Cumbia();
}

SimpleA::~SimpleA()
{
    delete m_cumbia;
}

void SimpleA::start()
{
    m_cnt++;
    CuData th_tok("name", "simplea_thread"); // thread token
    if(!findChild<QCheckBox *>("cbSameTh")->isChecked())
        m_thid = m_cnt; // change thread id so that a new thread is used for the activity
    th_tok["th_id"] = m_thid;
    findChild<QProgressBar *>()->setValue(0); // reset progress
    Activity *a = new Activity(CuData("name", "activity " + std::to_string(m_cnt)));
    m_cumbia->registerActivity(a, this, th_tok,
                               CuThreadFactoryImpl(),
                               QThreadsEventBridgeFactory());
}

void SimpleA::onProgress(int step, int /* total */, const CuData &data)
{
    QString main_th;
    main_th.sprintf("0x%lx", pthread_self());
    findChild<QProgressBar *>()->setValue(step);
    new QTreeWidgetItem(findChild<QTreeWidget *>(), QStringList()
                        << data["name"].toString().c_str()
            << data["msg"].toString().c_str()
            << data["thread"].toString().c_str() << main_th);
}

void SimpleA::onResult(const CuData &data)
{
    QString main_th;
    main_th.sprintf("0x%lx", pthread_self());
    new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << data["name"].toString().c_str()
            << data["msg"].toString().c_str()
            << data["thread"].toString().c_str() << main_th);

    if(data["msg"].toString() == "onExit")
    {
        CuThreadService *ts = static_cast<CuThreadService *> (m_cumbia->getServiceProvider()->get(CuServices::Thread));
    }
}

CuData SimpleA::getToken() const
{
    return CuData("activity", "simple_activity");
}
