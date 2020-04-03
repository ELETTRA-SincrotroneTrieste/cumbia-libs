#include "qth_widget.h"
#include "ui_qth_widget.h"

#include <QString>
#include <QtDebug>
#include <QTimer>

QthWidget::QthWidget(QWidget *parent)
    : QWidget(parent)
{
    bool thread = true;
    if(thread) {
        QThread *updater = new Thread(this);
        qobject_cast<Thread *>(updater)->start();
        connect(updater, SIGNAL(onUpdate(QString)), ui->label, SLOT(setText(QString)), Qt::QueuedConnection);
    }
    else {
        QTimer *t = new QTimer(this);
        t->setSingleShot(false);
        t->setInterval(1000);
        connect(t, SIGNAL(timeout()), this, SLOT(onTimeout()));
        connect(this, SIGNAL(timerTxtReady(QString)), ui->label, SLOT(setText(QString)));
        t->start();
    }
}

QthWidget::~QthWidget()
{
    findChild<Thread *>()->stop();
    findChild<Thread *>()->wait();
    delete findChild<Thread *>();
}

void QthWidget::onTimeout() {
    m_cnt++;
    emit timerTxtReady(QString::number(m_cnt));
}


Thread::Thread(QObject *parent) : QThread(parent){
    m_cnt = 0;
    m_stop = false;
}

void Thread::stop()
{
    m_stop = true;
}

void Thread::run() {
    while(!m_stop) {
        m_cnt++;
        emit onUpdate(QString::number(m_cnt));
        m_stop = true;
      //  QThread::msleep(1000);
    }
    qDebug() << __PRETTY_FUNCTION__ << "thread finished";
}
