#include "botmonitor.h"
#include "botconfig.h"

#include <QString>
#include <QMap>
#include <QDateTime>
#include <QVariant>
#include <cudata.h>
#include <botreader.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <QtDebug>

class BotMonitorPrivate {
public:
    QMultiMap<int, BotReader *> readersMap;
    bool err;
    QString msg;
    CumbiaPool *cu_pool;
    CuControlsFactoryPool ctrl_factory_pool;
    int ttl;
};

BotMonitor::~BotMonitor()
{
    printf("\e[1;31m~BotMonitor %p\e[0m\n", this);
}

BotMonitor::BotMonitor(QObject *parent,
                       CumbiaPool *cu_pool,
                       const CuControlsFactoryPool &fpool,
                       int time_to_live) : QObject(parent)
{
    d = new BotMonitorPrivate;
    d->err = false;
    d->ctrl_factory_pool = fpool;
    d->cu_pool = cu_pool;
    d->ttl = time_to_live;
}

bool BotMonitor::error() const
{
    return d->err;
}

QString BotMonitor::message() const
{
    return d->msg;
}

BotReader *BotMonitor::findReader(int chat_id, const QString &src, const QString &host) const
{
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        if(it.key() == chat_id && it.value()->source() == src && it.value()->host() == host)
            return *it;
    }
    printf("\e[1;31mreader for %d %s not found\e[0m\n", chat_id, qstoc(src));
    return nullptr;
}

BotReader *BotMonitor::findReaderByUid(int user_id, const QString &src, const QString& host) const
{
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        if(it.key() == user_id && it.value()->source() == src && it.value()->host() == host) {
                return *it;
        }
    }
    return nullptr;
}

QList<BotReader *> BotMonitor::readers() const
{
    return d->readersMap.values();
}

bool BotMonitor::stop(int chat_id, const QString &src)
{
    bool found = false;
    d->err = !d->readersMap.contains(chat_id);
    QMap<int, BotReader *>::iterator it = d->readersMap.begin();
    while(it != d->readersMap.end()) {
        if(it.key() == chat_id && (it.value()->source() == src || src.isEmpty()) ) {
            BotReader *r = (*it);
            emit stopped(r->userId(), chat_id, r->source(), r->host(), "user request");
            it.value()->deleteLater();
            it = d->readersMap.erase(it);
            found = true;
        }
        else
            ++it;
    }
    if(!found)
        d->msg = "BotMonitor.stop: source \"" + src + "\" not monitored";

    return !d->err && found;
}


bool BotMonitor::stopByIdx(int chat_id, int index)
{
    QString src;
    QList<BotReader *> readers = d->readersMap.values(chat_id);
    for(int i = 0; i < readers.size() && src.isEmpty(); i++) {
        BotReader *r = readers[i];
        if(r->index() == index)
            src = r->source();
    }
    if(!src.isEmpty())
        return stop(chat_id, src);
    d->msg = "BotMonitor.stopByIdx: no reader found with index " + QString::number(index);
    return false;
}

bool BotMonitor::startRequest(int user_id,
                              int chat_id,
                              const QString &src,
                              const QString& formula,
                              BotReader::Priority priority,
                              const QString &host,
                              const QDateTime& started_on)
{
    qDebug() << __PRETTY_FUNCTION__ << "startRequest with host " << host;
    d->err = false;
    BotReader *reader = findReader(chat_id, src, host);
    if(!reader) {
        bool monitor = true;
        BotReader *reader = new BotReader(user_id, chat_id, this,
                                          d->cu_pool, d->ctrl_factory_pool,
                                          d->ttl, formula, priority, host, monitor);
        connect(reader, SIGNAL(newData(int, const CuData&)), this, SLOT(m_onNewData(int, const CuData&)));
        connect(reader, SIGNAL(formulaChanged(int, QString,QString, QString)),
                this, SLOT(m_onFormulaChanged(int, QString,QString, QString)));
        connect(reader, SIGNAL(priorityChanged(int, const QString&, BotReader::Priority , BotReader::Priority)),
                this, SLOT(m_onPriorityChanged(int, const QString&, BotReader::Priority , BotReader::Priority)));
        connect(reader, SIGNAL(startSuccess(int, int, QString, QString)),
                this, SLOT(readerStartSuccess(int, int, QString, QString)));
        connect(reader, SIGNAL(lastUpdate(int, const CuData&)), this, SLOT(m_onLastUpdate(int, const CuData&)));
        reader->setSource(src);
        reader->setStartedOn(started_on);
        reader->setIndex(m_findIndexForNewReader(chat_id));

        // will be inserted in map upon successful "property" delivery
    }
    else {

        d->err = (formula == reader->formula()) && (priority == reader->priority() );

        if(!d->err && reader->formula() != formula)
            reader->setFormula(formula);

        if(!d->err && reader->priority() != priority)
            reader->setPriority(priority);

        if(d->err) {
            d->msg = "BotMonitor.startRequest: chat_id " + QString::number(chat_id) + " is already monitoring \"" + src;
            !reader->formula().isEmpty() ? (d->msg += " " + reader->formula() + "\"") : (d->msg += "\"");
            perr("%s", qstoc(d->msg));
        }
    }
    return !d->err;
}

void BotMonitor::readerStartSuccess(int user_id, int chat_id, const QString &src, const QString &formula)
{
    printf("\e[1;32m+++ readerStart success %d %d  %s %s \e[0m\n", user_id, chat_id, qstoc(src), qstoc(formula));
    BotReader *reader = qobject_cast<BotReader *>(sender());
    d->readersMap.insert(chat_id, reader);
    emit started(user_id, chat_id, src, reader->host(), formula);
}

void BotMonitor::m_onNewData(int chat_id, const CuData &da)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    const QString src = reader->source();
    const QString host = reader->host();
    const QString msg = QString::fromStdString(da["msg"].toString());
    d->err = da["err"].toBool();
    if(d->err) {
        perr("BotMonitor.m_onNewData: error chat_id %d msg %s", chat_id, qstoc(msg));
        emit stopped(reader->userId(), chat_id, src, host, msg);
    }
    // emit new data if it has a value
    else if(da.containsKey("value"))
        emit newData(chat_id, da);
}

void BotMonitor::m_onFormulaChanged(int chat_id, const QString &src, const QString &old, const QString &new_f)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    emit onFormulaChanged(reader->userId(), chat_id, src, reader->host(), old, new_f);
}

void BotMonitor::m_onPriorityChanged(int user_id, int chat_id, const QString &src, BotReader::Priority oldpri, BotReader::Priority newpri)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    QString oldtype, newtype;
    oldpri == BotReader::Low ? oldtype = "monitor" : oldtype = "alert";
    newpri == BotReader::Low ? newtype = "monitor" : newtype = "alert";
    emit onMonitorTypeChanged(user_id, chat_id, src, reader->host(), oldtype, newtype);
}

void BotMonitor::m_onLastUpdate(int chat_id, const CuData &)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    emit stopped(reader->userId(), chat_id, reader->source(), reader->host(), "end of TTL");
    reader->deleteLater();
    QMap<int, BotReader *>::iterator it = d->readersMap.begin();
    while(it != d->readersMap.end())
        it.value() == reader ? it =  d->readersMap.erase(it) : ++it;
}

int BotMonitor::m_findIndexForNewReader(int chat_id)
{
    QList<int> indexes;
    // readers map populated by readerStartSuccess. key is chat_id
    foreach(BotReader* r, d->readersMap.values(chat_id)) {
        if(r->index() > -1)
            indexes << r->index();
    }
    int i = 1;
    for(i = 1; i <= indexes.size(); i++)
        if(!indexes.contains(i)) // found a "hole"
            return i;
    return i;
}
