#include "cubotserver.h"
#include "cubotlistener.h"
#include "cubotsender.h"
#include "tbotmsg.h"
#include "tbotmsgdecoder.h"
#include "botreader.h"
#include "cumbiasupervisor.h"
#include "msgformatter.h"
#include "botmonitor.h"
#include "botconfig.h"

#include <cumacros.h>
#include <QtDebug>

#include <QJsonValue>

#include <cucontrolsfactorypool.h>
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>

#ifdef QUMBIA_EPICS_CONTROLS
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#include <cuepreadoptions.h>
#endif

class CuBotServerPrivate {
public:
    CuBotListener *bot_listener;
    CuBotSender *bot_sender;
    BotDb *bot_db;
    BotMonitor *bot_mon;
    CumbiaSupervisor cu_supervisor;
};

CuBotServer::CuBotServer(QObject *parent) : QObject(parent)
{
    d = new CuBotServerPrivate;
    d->bot_listener = nullptr;
    d->bot_sender = nullptr;
    d->bot_mon = nullptr;
}

CuBotServer::~CuBotServer()
{
    predtmp("~CuBotServer %p", this);
    stop();
    delete d;
}

void CuBotServer::onMessageReceived(const TBotMsg &m)
{
    bool success = true;
    m.print();
    int uid = m.user_id;

    // 1. see if user exists, otherwise add him
    if(!d->bot_db->userExists(uid)) {
        success = d->bot_db->addUser(uid, m.username, m.first_name, m.last_name);
        if(!success)
            perr("CuBotServer.onMessageReceived: error adding user with id %d: %s", uid, qstoc(d->bot_db->message()));
        else
            printf("\e[1;32m+\e[0m successfully added user \"%s\" with id %d\n", qstoc(m.username), uid);
    }

    TBotMsgDecoder msg_dec(m);
    //    printf("type of message is %s [%d]\n", msg_dec.types[msg_dec.type()], msg_dec.type());
    TBotMsgDecoder::Type t = msg_dec.type();
    if(t == TBotMsgDecoder::Host) {
        QString host = msg_dec.host();
        success = d->bot_db->setHost(m.user_id, m.chat_id, host);
        if(success)
            success = d->bot_db->insertOperation(HistoryEntry(m.user_id, m.text, "host")); // "host" is type

        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().hostChanged(host, success), true); // silent
        if(!success)
            perr("CuBotServer::onMessageReceived: database error: %s", qstoc(d->bot_db->message()));
    }
    else if(t == TBotMsgDecoder::QueryHost) {
        QString host = d->bot_db->getSelectedHost(m.chat_id);
        if(host.isEmpty())
            host = QString(secure_getenv("TANGO_HOST"));
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().host(host));
    }
    else if(t == TBotMsgDecoder::Last) {
        QDateTime dt;
        HistoryEntry he = d->bot_db->lastOperation(m.user_id);
        if(he.isValid()) {
            TBotMsg lm = m;
            lm.text = he.name;
            lm.setHost(he.host);
            if(he.hasFormula())
                lm.text += " " + he.formula;
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().lastOperation(he.datetime, lm.text));
            //
            // call ourselves with the updated copy of the received message
            //
            onMessageReceived(lm);
        }
    }
    else if(t == TBotMsgDecoder::Read) {
        QString src = msg_dec.source();
        QString host; // if m.hasHost then m comes from a fake message created ad hoc by Last: use this host
        m.hasHost() ? host = m.host() : host = d->bot_db->getSelectedHost(m.chat_id); // may be empty. If so, TANGO_HOST will be used
        BotReader *r = new BotReader(m.user_id, m.chat_id, this, d->cu_supervisor.cu_pool,
                                     d->cu_supervisor.ctrl_factory_pool,
                                     msg_dec.formula(), BotReader::Normal, host);
        connect(r, SIGNAL(newData(int, const CuData&)), this, SLOT(onNewData(int, const CuData& )));
        r->setPropertyEnabled(false);
        r->setSource(src); // insert in  history db only upon successful connection
    }
    else if(t == TBotMsgDecoder::Monitor || t == TBotMsgDecoder::Alert) {
        QString src = msg_dec.source();
        if(!d->bot_mon)
            m_setupMonitor(); // insert in history db only upon successful connection
        QString host; // if m.hasHost use it, it comes from a fake history message created ad hoc by History
        m.hasHost() ? host = m.host() : host = d->bot_db->getSelectedHost(m.chat_id); // may be empty. If so, TANGO_HOST will be used
        success = d->bot_mon->startRequest(m.user_id, m.chat_id, src, msg_dec.formula(),
                                           t == TBotMsgDecoder::Monitor ? BotReader::Low : BotReader::Normal,
                                           host);
        if(!success)
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("CuBotServer", d->bot_mon->message()));
    }
    else if(t == TBotMsgDecoder::StopMonitor) {
        QString src = msg_dec.source();
        if(d->bot_mon) {
            success = d->bot_mon->stop(m.chat_id, src);
            if(!success) {
                d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("CuBotServer", d->bot_mon->message()));
                // failure in this phase means reader is already stopped (not in monitor's map).
                // make sure it is deleted from the database too
                // ..
            }
        }
    }
    else if(t == TBotMsgDecoder::ReadHistory || t == TBotMsgDecoder::MonitorHistory ||
            t == TBotMsgDecoder::AlertHistory) {
        QList<HistoryEntry> hel = m_prepareHistory(d->bot_db->history(m.user_id), t);
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().history(hel));
    }
    else if(t == TBotMsgDecoder::CmdLink) {
        int cmd_idx = msg_dec.cmdLinkIdx();
        if(cmd_idx > -1) {
            QDateTime dt;
            QString operation;
            HistoryEntry he = d->bot_db->commandFromIndex(m.user_id, m.text, cmd_idx);
            if(he.isValid()) {
                operation = he.toCommand();
                // 1. remind the user what was the command linked to /commandN
                d->bot_sender->sendMessage(m.chat_id, MsgFormatter().lastOperation(dt, operation));
                // 2.
                // call ourselves with the updated copy of the received message
                //
                TBotMsg lnkm = m;
                lnkm.text = operation;
                lnkm.setHost(he.host);
                onMessageReceived(lnkm);
            }
        }
    }
}

void CuBotServer::onNewData(int chat_id, const CuData &data)
{
    MsgFormatter mf;
    d->bot_sender->sendMessage(chat_id, mf.fromData(data));
    BotReader *reader = qobject_cast<BotReader *>(sender());
    HistoryEntry he(reader->userId(), reader->source(), "read", reader->formula(), reader->host());
    d->bot_db->insertOperation(he);
}

/**
 * @brief CuBotServer::onNewMonitorData send message on new monitored data
 *
 * Message is sent silently
 *
 * @param chat_id
 * @param da
 *
 */
void CuBotServer::onNewMonitorData(int chat_id, const CuData &da)
{
    MsgFormatter mf;
    d->bot_sender->sendMessage(chat_id, mf.fromData(da), da["silent"].toBool());
    d->bot_db->readUpdate(chat_id, mf.source(), mf.value(), mf.qualityString());
}

void CuBotServer::onSrcMonitorStopped(int chat_id, const QString &src, const QString &message)
{
    const bool silent = true;
    const QString msg = "stopped monitoring " + src + ": " + message;
    d->bot_sender->sendMessage(chat_id, msg, silent);
    // update database, remove rows for chat_id and src
    d->bot_db->monitorStopped(chat_id, src);
}

void CuBotServer::onSrcMonitorStarted(int user_id, int chat_id, const QString &src, const QString& formula)
{
    const bool silent = true;
    const QString until = QDateTime::currentDateTime().addSecs(BotConfig().ttl()).toString("MM.dd hh:mm:ss");
    const QString msg = "started monitoring " + src + " until " + until;
    BotReader *r = d->bot_mon->findReader(chat_id, src);
    BotReader::Priority pri = r->priority();
    const QString host = r->host();
    d->bot_sender->sendMessage(chat_id, msg, silent);
    HistoryEntry he(user_id, src, pri == BotReader::Normal ? "alert" :  "monitor", formula, r->host());
    d->bot_db->insertOperation(he);
}

void CuBotServer::onSrcMonitorAlarm(int chat_id, const CuData &da)
{

}

void CuBotServer::start()
{
    d->cu_supervisor.setup();
    d->bot_db = new BotDb();
    if(d->bot_db->error())
        perr("CuBotServer.start: error opening QSQLITE telegram bot db: %s", qstoc(d->bot_db->message()));

    if(!d->bot_listener) {
        d->bot_listener = new CuBotListener(this);
        connect(d->bot_listener, SIGNAL(onNewMessage(const TBotMsg &)),
                this, SLOT(onMessageReceived(const TBotMsg&)));
        d->bot_listener->start();
    }
    if(!d->bot_sender) {
        d->bot_sender = new CuBotSender(this);
    }
}

void CuBotServer::stop()
{
    if(d->bot_listener) {
        d->bot_listener->stop(); // deleted when this is deleted
    }
    if(d->bot_db)
        delete d->bot_db;
    d->bot_db = nullptr;
    d->cu_supervisor.dispose();
}

void CuBotServer::onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &src, const QString &old, const QString &new_f)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().formulaChanged(src, old, new_f));
    BotReader *r = d->bot_mon->findReader(chat_id, src);
    HistoryEntry he(user_id, src, r->priority() == BotReader::Low ? "monitor" : "alert", new_f);
    d->bot_db->insertOperation(he);
}

void CuBotServer::m_setupMonitor()
{
    if(!d->bot_mon) {
        d->bot_mon = new BotMonitor(this, d->cu_supervisor.cu_pool, d->cu_supervisor.ctrl_factory_pool);
        connect(d->bot_mon, SIGNAL(newData(int, const CuData&)), this, SLOT(onNewMonitorData(int, const CuData&)));
        connect(d->bot_mon, SIGNAL(stopped(int, QString, QString)), this, SLOT(onSrcMonitorStopped(int, QString, QString)));
        connect(d->bot_mon, SIGNAL(started(int,int, QString,QString)), this, SLOT(onSrcMonitorStarted(int,int, QString,QString)));
        connect(d->bot_mon, SIGNAL(alarm(int, const CuData&)), this, SLOT(onSrcMonitorAlarm(int, const CuData&)));
        connect(d->bot_mon, SIGNAL(onFormulaChanged(int, int, QString,QString,QString)),
                this, SLOT(onSrcMonitorFormulaChanged(int, int, QString,QString,QString)));
    }
}

/**
 * @brief CuBotServer::m_prepareHistory modify the list of HistoryEntry so that it can be pretty
 *        printed by MsgFormatter before sending.
 * @param in the list of history entries as returned by the database operations table
 * @return the list of history entries with some fields modified according to the state of some entries
 *         (running, not running, and so on)
 *
 * The purpose is to provide an interactive list where stopped monitors and alerts can be easily restarted
 * through a link, while running ones are just listed
 *
 */
QList<HistoryEntry> CuBotServer::m_prepareHistory(const QList<HistoryEntry> &in, TBotMsgDecoder::Type t)
{
    QList<HistoryEntry> out;
    foreach(HistoryEntry he, in) {
        if(t == TBotMsgDecoder::MonitorHistory && he.type == "monitor") {
            he.is_active = d->bot_mon && d->bot_mon->findReaderByUid(he.user_id, he.name) != nullptr;
            out << he;
        }
        else if(t == TBotMsgDecoder::AlertHistory && he.type == "alert") {
            he.is_active = d->bot_mon && d->bot_mon->findReaderByUid(he.user_id, he.name) != nullptr;
            out << he;
        }
        else if(t == TBotMsgDecoder::ReadHistory && he.type == "read") {
            out << he;
        }
    }
    return out;
}
