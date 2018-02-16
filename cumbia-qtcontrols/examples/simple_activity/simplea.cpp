#include "simplea.h"
#include <cumbia.h>
#include <QProgressBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>

SimpleA::SimpleA(QWidget *parent) : QWidget(parent)
{
   QVBoxLayout *vlo = new QVBoxLayout(this); // layout in two rows
   QProgressBar *pb = new QProgressBar(this); // progress bar
   pb->setMinimum(0); pb->setMaximum(3); pb->setValue(0);
   vlo->addWidget(pb);
   QTreeWidget *tw = new QTreeWidget(this);
   tw->setHeaderLabels(QStringList() << "message" << "thread");
   vlo->addWidget(tw);
   QPushButton *pbu = new QPushButton("start", this);
   connect(pbu, SIGNAL(clicked()), this, SLOT(start()));
   vlo->addWidget(pbu);
}

void SimpleA::start()
{
    findChild<QProgressBar *>()->setValue(0);
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->current_activity, this, tt, fi, bf);
}

void SimpleA::onProgress(int step, int /* total */, const CuData &data)
{
    QString thread, main_th;
    thread.sprintf("0x%lx", data["thread"].toULongInt());
    main_th.sprintf("0x%lx", pthread_self());
    findChild<QProgressBar *>()->setValue(step);
    new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << thread);
}

void SimpleA::onResult(const CuData &data)
{
    QString thread, main_th;
    thread.sprintf("0x%lx", data["thread"].toULongInt());
    main_th.sprintf("0x%lx", pthread_self());
    QTreeWidgetItem *tw = new QTreeWidgetItem(findChild<QTreeWidget *>(),
                        QStringList() << QString::fromStdString(data["msg"].toString())
            << thread);
    tw->setBackgroundColor(0, QColor(Qt::green));
}

CuData SimpleA::getToken() const
{
    printf("SimpleA::getToken is called\n");
    return CuData("activity", "simple_activity");
}
