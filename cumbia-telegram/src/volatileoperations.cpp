#include "volatileoperations.h"
#include <QtDebug>
#include <QTimer>

VolatileOperations::VolatileOperations()
{
    QTimer * cleanTimer = new QTimer(this);
    cleanTimer->setSingleShot(false);
    cleanTimer->setInterval(1000);
    connect(cleanTimer, SIGNAL(timeout()), this, SLOT(cleanOld()));
}

VolatileOperations::~VolatileOperations()
{
    foreach(VolatileOperation *vop, m_op_map.values())
        delete vop;
    m_op_map.clear();
}

void VolatileOperations::addOperation(int chat_id, VolatileOperation *op)
{
    if(m_op_map.size() == 0)
        findChild<QTimer *>()->start();

    m_op_map.insert(chat_id, op);
}

void VolatileOperations::consume(int chat_id, TBotMsgDecoder::Type t)
{
    qDebug() << __PRETTY_FUNCTION__ << m_op_map;
    QList<VolatileOperation *> voplist = m_op_map.values(chat_id);
    QList<VolatileOperation *> to_remove; // operations slated for removal: avoid QMutableIterator
    foreach(VolatileOperation *vop, voplist) {
        vop->consume(t);
        if(vop->lifeCount() < 0) {
            to_remove << vop;
        }
    }
    // remove
    foreach(VolatileOperation *vo, to_remove) {
        m_op_map.remove(chat_id, vo);
        delete vo;
    }

    if(m_op_map.size() == 0)
        findChild<QTimer *>()->stop();
}

VolatileOperation *VolatileOperations::get(int chat_id, int type) const
{
    qDebug() << __PRETTY_FUNCTION__ << m_op_map;
    QList<VolatileOperation *> voplist = m_op_map.values(chat_id);
    foreach(VolatileOperation *vop, voplist) {
        qDebug() << __PRETTY_FUNCTION__ << chat_id << "type" << type << "vop type" << vop->type();
        if(vop->type() == type)
            return vop;
    }
    if(!m_op_map.contains(type))
        qDebug() << __PRETTY_FUNCTION__ << "no operations with type " << type;
    return nullptr;
}

void VolatileOperations::cleanOld()
{
    qDebug() << __PRETTY_FUNCTION__ << "timeout: cleanOld invoketh";
    foreach(int key, m_op_map.keys()) {
        QList<VolatileOperation *> voplist = m_op_map.values(key);
        QList<VolatileOperation *> to_remove;
        QDateTime now = QDateTime::currentDateTime();
        foreach(VolatileOperation *vo, voplist) {
            if(vo->creationTime().secsTo(now) > vo->ttl())
                to_remove << vo;
        }
        foreach(VolatileOperation *rvo,  to_remove) {
            rvo->signalTtlExpired();
            m_op_map.remove(key, rvo);
            delete rvo;
        }
    }

    if(m_op_map.size() == 0)
        findChild<QTimer *>()->stop();
}
