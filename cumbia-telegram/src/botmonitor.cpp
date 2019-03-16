#include "botmonitor.h"
#include "botconfig.h"
#include "monitorhelper.h"
#include <QString>
#include <QMap>
#include <QDateTime>
#include <QVariant>
#include <cudata.h>
#include <botreader.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cuformulaparsehelper.h>
#include <QtDebug>

class BotMonitorPrivate {
public:
    QMultiMap<int, BotReader *> readersMap;
    bool err;
    QString msg;
    CumbiaPool *cu_pool;
    CuControlsFactoryPool ctrl_factory_pool;
    int ttl, poll_period, max_avg_poll_period;
};

BotMonitor::~BotMonitor()
{
    printf("\e[1;31m~BotMonitor %p\e[0m\n", this);
}

BotMonitor::BotMonitor(QObject *parent,
                       CumbiaPool *cu_pool,
                       const CuControlsFactoryPool &fpool,
                       int time_to_live, int poll_period) : QObject(parent)
{
    d = new BotMonitorPrivate;
    d->err = false;
    d->ctrl_factory_pool = fpool;
    d->cu_pool = cu_pool;
    d->ttl = time_to_live;
    d->poll_period = poll_period;
    d->max_avg_poll_period = 1000;
}

bool BotMonitor::error() const
{
    return d->err;
}

QString BotMonitor::message() const
{
    return d->msg;
}

BotReader *BotMonitor::findReader(int chat_id, const QString &expression, const QString &host) const
{
    CuFormulaParseHelper ph;
    QSet<QString> srcs = ph.sources(expression).toSet(); // extract sources from expression
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        qDebug() << __PRETTY_FUNCTION__ << "find srcs" << srcs << "reader srcs" << it.value()->sources() <<
                    "find host" << host << " reader host " << it.value()->host();
        if(it.key() == chat_id && it.value()->sameSourcesAs(srcs) && it.value()->host() == host)
            return *it;
    }
    printf("\e[1;31mreader for %d %s not found\e[0m\n", chat_id, qstoc(expression));
    return nullptr;
}

BotReader *BotMonitor::findReaderByUid(int user_id, const QString &expression, const QString& host) const
{
    CuFormulaParseHelper ph;
    QString expr = ph.toNormalizedForm(expression);
    QSet<QString> srcs = ph.sources(expr).toSet(); // extract sources from expression
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        if(it.key() == user_id && it.value()->sameSourcesAs(srcs) && it.value()->host() == host) {
            return *it;
        }
    }
    return nullptr;
}

QList<BotReader *> BotMonitor::readers() const
{
    return d->readersMap.values();
}

void BotMonitor::setMaxAveragePollingPeriod(int millis)
{
    d->max_avg_poll_period = millis;
}

int BotMonitor::maxAveragePollingPeriod() const
{
    return d->max_avg_poll_period;
}

bool BotMonitor::stopAll(int chat_id, const QStringList &srcs)
{
    qDebug() << __PRETTY_FUNCTION__ << chat_id << srcs;
    bool found = false;
    d->err = !d->readersMap.contains(chat_id);
    QMutableMapIterator<int, BotReader *> it(d->readersMap);
    while(it.hasNext()) {
        it.next();
        if(it.key() == chat_id) {
            BotReader *r = it.value();
            bool delet = srcs.isEmpty();
            for(int i = 0; i < srcs.size() && !delet; i++) {
                delet = r->sourceMatch(srcs[i]);
                qDebug() << __PRETTY_FUNCTION__ << chat_id << "fickin match" << srcs[i] << delet;
            }
            if(delet) {
                emit stopped(r->userId(), chat_id, r->source(), r->host(), "user request");
                r->deleteLater();
                it.remove();
                found = true;
            }
        }
    }
    if(!found)
        d->msg = "BotMonitor.stop: none of the sources matching one of the patterns"
                 " \"" + srcs.join(", ") + "\" are monitored";
    return !d->err && found;
}


bool BotMonitor::stopByIdx(int chat_id, int index)
{
    bool found = false;
    QMutableMapIterator<int, BotReader *> it(d->readersMap);
    while(it.hasNext() && !found) {
        it.next();
        if(it.key() == chat_id) {
            BotReader *r = it.value();
            if(r->index() == index) {
                emit stopped(r->userId(), chat_id, r->source(), r->host(), "user request");
                r->deleteLater();
                it.remove();
                found = true;
            }
        }
    }
    if(!found)
        d->msg = "BotMonitor.stopByIdx: no reader found with index " + QString::number(index);
    return found;
}

bool BotMonitor::startRequest(int user_id,
                              int chat_id,
                              int uid_monitor_limit,
                              const QString &src,
                              const QString& cmd,
                              BotReader::Priority priority,
                              const QString &host,
                              const QDateTime& started_on)
{
    qDebug() << __PRETTY_FUNCTION__ << "startRequest with host " << host << "SOURCE " << src;
    d->err = false;
    BotReader *reader = findReader(chat_id, src, host);
    d->err = (reader && src == reader->source() && priority == reader->priority() );
    if(d->err){
        d->msg = "BotMonitor.startRequest: chat_id " + QString::number(chat_id) + " is already monitoring \"" + cmd;
        !reader->command().isEmpty() ? (d->msg += " " + reader->command() + "\"") : (d->msg += "\"");
        perr("%s", qstoc(d->msg));
    }
    else if( reader && src == reader->source() ) { // same source but priority changed
        reader->setPriority(priority);
    }
    else if(reader) {
        QString oldcmd = reader->command();
        reader->unsetSource();
        reader->setCommand(cmd);
        reader->setSource(src);
        m_onFormulaChanged(user_id, chat_id, reader->sources().join(","), oldcmd, reader->command(), host);
    }
    else {
        int uid_readers_cnt = d->readersMap.values(chat_id).size();
        if(uid_readers_cnt < uid_monitor_limit) {
            bool monitor = true;
            BotReader *reader = new BotReader(user_id, chat_id, this,
                                              d->cu_pool, d->ctrl_factory_pool,
                                              d->ttl, d->poll_period, cmd, priority, host, monitor);
            connect(reader, SIGNAL(newData(int, const CuData&)), this, SLOT(m_onNewData(int, const CuData&)));
            connect(reader, SIGNAL(formulaChanged(int, int, QString,QString, QString,QString)),
                    this, SLOT(m_onFormulaChanged(int, int, QString,QString, QString,QString)));
            connect(reader, SIGNAL(priorityChanged(int, const QString&, BotReader::Priority , BotReader::Priority)),
                    this, SLOT(m_onPriorityChanged(int, const QString&, BotReader::Priority , BotReader::Priority)));
            connect(reader, SIGNAL(startSuccess(int, int, QString, QString)),
                    this, SLOT(readerStartSuccess(int, int, QString, QString)));
            connect(reader, SIGNAL(lastUpdate(int, const CuData&)), this, SLOT(m_onLastUpdate(int, const CuData&)));
            connect(reader, SIGNAL(modeChanged(BotReader::RefreshMode)), this, SLOT(m_onReaderModeChanged(BotReader::RefreshMode)));
            reader->setSource(src);
            reader->setStartedOn(started_on);
            reader->setIndex(m_findIndexForNewReader(chat_id));
        }
        else {
            emit startError(chat_id, src, QString("limit of %1 monitors already reached").arg(uid_monitor_limit));
        }

        // will be inserted in map upon successful "property" delivery
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
        emit newMonitorData(chat_id, da);
}

void BotMonitor::m_onFormulaChanged(int user_id, int chat_id, const QString &src, const QString &old, const QString &new_f, const QString& host)
{
    emit onFormulaChanged(user_id, chat_id, src, host, old, new_f);
}

void BotMonitor::m_onPriorityChanged(int chat_id, const QString &src, BotReader::Priority oldpri, BotReader::Priority newpri)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    int user_id = reader->userId();
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

void BotMonitor::m_onReaderModeChanged(BotReader::RefreshMode rm)
{
    BotReader *r = qobject_cast<BotReader *>(sender());
    emit readerRefreshModeChanged(r->userId(), r->chatId(), r->source(), r->host(), rm);
}
