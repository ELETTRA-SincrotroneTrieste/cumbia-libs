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
    m_op_map.insertMulti(chat_id, op);
}

/**
 * @brief VolatileOperations::replaceOperation replaces operations with the same type as op with
 *        op
 * @param chat_id the chat id where the VolatileOperation has to be replaced
 * @param op the new operation. op type is used to find other operations with the same type
 */
void VolatileOperations::replaceOperation(int chat_id, VolatileOperation *op)
{
    QMutableMapIterator<int, VolatileOperation*> i(m_op_map);
    while(i.hasNext()) {
        i.next();
        if(i.key() == chat_id) {
            VolatileOperation *vop = i.value();
            if(vop->type() == op->type()) {
                i.remove();
                delete vop;
            }
        }
    }
    addOperation(chat_id, op);
}

void VolatileOperations::consume(int chat_id, TBotMsgDecoder::Type t)
{
    QMutableMapIterator<int, VolatileOperation*> i(m_op_map);
    while(i.hasNext()) {
        i.next();
        if(i.key() == chat_id) {
            VolatileOperation *vop = i.value();
            vop->consume(t);
            if(vop->lifeCount() < 0) {
                i.remove();
                delete vop;
            }
        }
    }

    if(m_op_map.size() == 0)
        findChild<QTimer *>()->stop();
}

VolatileOperation *VolatileOperations::get(int chat_id, int type) const
{
    QList<VolatileOperation *> voplist = m_op_map.values(chat_id);
    foreach(VolatileOperation *vop, voplist) {
        if(vop->type() == type)
            return vop;
    }
    if(!m_op_map.contains(type)) {
    }

    return nullptr;
}

void VolatileOperations::cleanOld()
{
    QMutableMapIterator<int, VolatileOperation*> i(m_op_map);
    QDateTime now = QDateTime::currentDateTime();
    while(i.hasNext()) {
        i.next();
        VolatileOperation *vop = i.value();
        if(vop->creationTime().secsTo(now) > vop->ttl()) {
            vop->signalTtlExpired();
            i.remove();
            delete vop;
        }
    }
    if(m_op_map.size() == 0)
        findChild<QTimer *>()->stop();
}
