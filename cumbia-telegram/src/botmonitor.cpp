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

BotMonitor::BotMonitor(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fpool) : QObject(parent)
{
    d = new BotMonitorPrivate;
    d->err = false;
    d->ctrl_factory_pool = fpool;
    d->cu_pool = cu_pool;
    d->ttl = BotConfig().ttl();
}

bool BotMonitor::error() const
{
    return d->err;
}

QString BotMonitor::message() const
{
    return d->msg;
}

BotReader *BotMonitor::findReader(int chat_id, const QString &src) const
{
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        qDebug() << __FUNCTION__ << "comparing \"" << it.value()->source() << "\" wit \""  << src << "\"";
        if(it.key() == chat_id && it.value()->source() == src)
            return *it;
    }
    printf("\e[1;31mreader for %d %s not found\e[0m\n", chat_id, qstoc(src));
    return nullptr;
}

BotReader *BotMonitor::findReaderByUid(int user_id, const QString &src) const
{
    for(QMap<int, BotReader *>::iterator it = d->readersMap.begin(); it != d->readersMap.end(); ++it) {
        if(it.key() == user_id && it.value()->source() == src)
            return *it;
    }
    return nullptr;
}

bool BotMonitor::stop(int chat_id, const QString &src)
{
    bool found = false;
    d->err = !d->readersMap.contains(chat_id);
    QStringList srcs;
    QMap<int, BotReader *>::iterator it = d->readersMap.begin();
    while(it != d->readersMap.end()) {
        if(it.key() == chat_id && (it.value()->source() == src || src.isEmpty()) ) {
            srcs << it.value()->source();
            it.value()->deleteLater();
            it = d->readersMap.erase(it);
            found = true;
        }
        else
            ++it;
    }
    if(!found)
        d->msg = "BotMonitor.stop: source \"" + src + "\" not monitored";
    else
        emit stopped(chat_id, srcs.join(" "), "as to user request");

    return !d->err && found;
}

bool BotMonitor::startRequest(int user_id,
                              int chat_id,
                              const QString &src,
                              const QString& formula,
                              BotReader::Priority priority, const QString &host)
{
    d->err = false;
    BotReader *reader = findReader(chat_id, src);
    if(!reader) {
        bool monitor = true;
        BotReader *reader = new BotReader(user_id, chat_id, this, d->cu_pool, d->ctrl_factory_pool,
                                          formula, priority, host, monitor);
        connect(reader, SIGNAL(newData(int, const CuData&)), this, SLOT(m_onNewData(int, const CuData&)));
        connect(reader, SIGNAL(formulaChanged(int, QString,QString, QString)),
                this, SLOT(m_onFormulaChanged(int, QString, QString, QString)));
        connect(reader, SIGNAL(startSuccess(int, int, QString, QString)),
                this, SLOT(readerStartSuccess(int, int, QString, QString)));
        reader->setSource(src);
        // will be inserted in map upon successful "property" delivery
    }
    else {
        if(reader->formula() != formula)
            reader->setFormula(formula);
        else
            d->err = true;

        if(reader->priority() != priority)
            reader->setPriority(priority);
        else
            d->err = true;

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
    reader->setProperty("startDt", QDateTime::currentDateTime());
    emit started(user_id, chat_id, src, formula);
}

void BotMonitor::m_onNewData(int chat_id, const CuData &da)
{
    qDebug() << __FUNCTION__ << "received" << da.toString().c_str();
    BotReader *reader = qobject_cast<BotReader *>(sender());
    const QString src = QString::fromStdString(da["src"].toString());
    const QString msg = QString::fromStdString(da["msg"].toString());
    d->err = da["err"].toBool();
    if(d->err) {
        perr("BotMonitor.m_onNewData: error chat_id %d msg %s", chat_id, qstoc(msg));
        emit stopped(chat_id, src, msg);
    }
    // emit new data if it has a value
    else if(da.containsKey("value"))
        emit newData(chat_id, da);

    // time to live?
    if(reader->property("startDt").toDateTime().secsTo(QDateTime::currentDateTime()) > d->ttl) {
        reader->deleteLater();
        QMap<int, BotReader *>::iterator it = d->readersMap.begin();
        while(it != d->readersMap.end())
            it.value() == reader ? it =  d->readersMap.erase(it) : ++it;
        emit stopped(chat_id, src, "end of TTL");
    }
}

void BotMonitor::m_onFormulaChanged(int chat_id, const QString &src, const QString &old, const QString &new_f)
{
    BotReader *reader = qobject_cast<BotReader *>(sender());
    emit onFormulaChanged(reader->userId(), chat_id, src, old, new_f);
}
