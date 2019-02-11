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
    BotConfig *botconf;
};

CuBotServer::CuBotServer(QObject *parent) : QObject(parent)
{
    d = new CuBotServerPrivate;
    d->bot_listener = nullptr;
    d->bot_sender = nullptr;
    d->bot_mon = nullptr;
    d->botconf = nullptr;
    d->bot_db = nullptr;
}

CuBotServer::~CuBotServer()
{
    predtmp("~CuBotServer %p", this);
    if(isRunning())
        stop();
    delete d;
}

bool CuBotServer::isRunning() const
{
    return d->bot_db != nullptr;
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
    }

    TBotMsgDecoder msg_dec(m);
    //    printf("type of message is %s [%d]\n", msg_dec.types[msg_dec.type()], msg_dec.type());
    TBotMsgDecoder::Type t = msg_dec.type();
    if(t == TBotMsgDecoder::Host) {
        QString host = msg_dec.host();
        success = d->bot_db->setHost(m.user_id, m.chat_id, host);
        if(success)
            success = d->bot_db->addToHistory(HistoryEntry(m.user_id, m.text, "host", "", "")); // "host" is type

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
                                     d->cu_supervisor.ctrl_factory_pool, d->botconf->ttl(),
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
        // m.start_dt will be invalid if m is decoded by a real message
        // m.start_dt is forced to a given date and time when m is a fake msg built
        // from the database history
        success = d->bot_mon->startRequest(m.user_id, m.chat_id, src, msg_dec.formula(),
                                           t == TBotMsgDecoder::Monitor ? BotReader::Low : BotReader::Normal,
                                           host, m.start_dt);
        if(!success)
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("CuBotServer", d->bot_mon->message()));
    }
    else if(t == TBotMsgDecoder::StopMonitor && msg_dec.cmdLinkIdx() < 0) {
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
    else if(t == TBotMsgDecoder::StopMonitor && msg_dec.cmdLinkIdx() > 0) {
        // stop by reader index!
        if(d->bot_mon)
            success = d->bot_mon->stopByIdx(m.chat_id, msg_dec.cmdLinkIdx());
        if(!success) {
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("CuBotServer", d->bot_mon->message()));
        }
    }
    else if(t == TBotMsgDecoder::ReadHistory || t == TBotMsgDecoder::MonitorHistory ||
            t == TBotMsgDecoder::AlertHistory || t == TBotMsgDecoder::Bookmarks) {
        QList<HistoryEntry> hel = m_prepareHistory(m.user_id, t);
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().history(hel, d->botconf->ttl(), msg_dec.toHistoryTableType(t)));
    }
    else if(t == TBotMsgDecoder::AddBookmark) {
        HistoryEntry he = d->bot_db->bookmarkLast(m.user_id);
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().bookmarkAdded(he));
    }
    else if(t == TBotMsgDecoder::DelBookmark) {
        int cmd_idx = msg_dec.cmdLinkIdx();
        if(cmd_idx > 0 && (success = d->bot_db->removeBookmark(m.user_id, cmd_idx))) {
                d->bot_sender->sendMessage(m.chat_id, MsgFormatter().bookmarkRemoved(success));
        }
    }
    else if(t == TBotMsgDecoder::Search) {
        printf("\e[0;32msearching \"%s\"\e[0m\n", qstoc(msg_dec.source()));

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
    d->bot_db->addToHistory(he);
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
}

void CuBotServer::onSrcMonitorStopped(int user_id, int chat_id, const QString &src,
                                      const QString &host, const QString &message)
{
    const bool silent = true;
    const QString msg = "stopped monitoring " + src + ": " + message;
    d->bot_sender->sendMessage(chat_id, msg, silent);
    // update database, remove rows for chat_id and src
    d->bot_db->monitorStopped(chat_id, src);
}

void CuBotServer::onSrcMonitorStarted(int user_id, int chat_id, const QString &src,
                                      const QString& host, const QString& formula)
{
    const bool silent = true;
    const QString until = QDateTime::currentDateTime().addSecs(d->botconf->ttl()).toString("MM.dd hh:mm:ss");
    const QString msg = "started monitoring " + src + " until " + until;
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    BotReader::Priority pri = r->priority();
    d->bot_sender->sendMessage(chat_id, msg, silent);
    // record new monitor into the database
    qDebug() << __PRETTY_FUNCTION__ << "adding history entry with formula " << formula << "host " << r->host();
    HistoryEntry he(user_id, src, pri == BotReader::Normal ? "alert" :  "monitor", formula, host);
    d->bot_db->addToHistory(he);
}

void CuBotServer::start()
{
    if(d->bot_db)
        perr("CuBotServer.start: already started\n");
    else {
        d->cu_supervisor.setup();
        d->bot_db = new BotDb();
        d->botconf = new BotConfig(d->bot_db);
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

        m_restoreProcs();
    }
}

void CuBotServer::stop()
{
    if(!d->bot_db)
        perr("CuBotServer.stop: already stopped\n");
    else {
        if(d->bot_listener) {
            d->bot_listener->stop();
            delete d->bot_listener;
            d->bot_listener = nullptr;
        }

        m_saveProcs();

        if(d->bot_db) {
            delete d->bot_db;
        }
        if(d->bot_mon) {
            foreach(BotReader *r, d->bot_mon->readers())
                delete r;
        }
        d->bot_db = nullptr;
        d->cu_supervisor.dispose();
    }
}

void CuBotServer::onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &src, const QString& host,
                                             const QString &old, const QString &new_f)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().formulaChanged(src, old, new_f));
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    HistoryEntry he(user_id, src, r->priority() == BotReader::Low ? "monitor" : "alert", new_f, r->host());
    d->bot_db->addToHistory(he);
}

void CuBotServer::onSrcMonitorTypeChanged(int user_id, int chat_id, const QString &src,
                                          const QString& host, const QString &old_type, const QString &new_type)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().monitorTypeChanged(src, old_type, new_type));
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    HistoryEntry he(user_id, src, new_type, r->formula(), r->host());
    d->bot_db->addToHistory(he);
}

void CuBotServer::m_setupMonitor()
{
    if(!d->bot_mon) {
        d->bot_mon = new BotMonitor(this, d->cu_supervisor.cu_pool, d->cu_supervisor.ctrl_factory_pool, d->botconf->ttl());
        connect(d->bot_mon, SIGNAL(newData(int, const CuData&)), this, SLOT(onNewMonitorData(int, const CuData&)));
        connect(d->bot_mon, SIGNAL(stopped(int, int, QString, QString, QString)),
                this, SLOT(onSrcMonitorStopped(int, int, QString, QString, QString)));
        connect(d->bot_mon, SIGNAL(started(int,int, QString,QString,QString)), this, SLOT(onSrcMonitorStarted(int,int, QString,QString,QString)));
        connect(d->bot_mon, SIGNAL(alarm(int, const CuData&)), this, SLOT(onSrcMonitorAlarm(int, const CuData&)));
        connect(d->bot_mon, SIGNAL(onFormulaChanged(int, int, QString,QString,QString,QString)),
                this, SLOT(onSrcMonitorFormulaChanged(int, int, QString,QString,QString,QString)));
        connect(d->bot_mon, SIGNAL(onSrcMonitorTypeChanged(int,int, QString,QString,QString,QString)),
                this, SLOT(onSrcMonitorTypeChanged(int,int, QString,QString,QString,QString)));
    }
}

bool CuBotServer::m_saveProcs()
{
    if(d->bot_mon) {
        foreach(BotReader *r, d->bot_mon->readers()) {
            HistoryEntry he(r->userId(), r->source(), r->priority() == BotReader::Normal ? "alert" :  "monitor",
                            r->formula(), r->host());
            he.chat_id = r->chatId(); // chat_id is needed to restore process at restart
            he.datetime = r->startedOn();
            d->bot_db->saveProc(he);
        }
    }
    return true;
}

bool CuBotServer::m_restoreProcs()
{
    bool success = true;
    QList<HistoryEntry> hes = d->bot_db->loadProcs();
    m_removeExpiredProcs(hes);
    if(d->bot_db->error())
        perr("CuBotServer:m_restoreProcs: database error: \"%s\"", qstoc(d->bot_db->message()));
    for(int i =0; i < hes.size() && !d->bot_db->error(); i++) {
        const HistoryEntry& he = hes[i];
//        printf("restoring proc %s type %s host %s formula %s chat id %d\n", qstoc(he.name), qstoc(he.type), qstoc(he.host), qstoc(he.formula), he.chat_id);
        onMessageReceived(TBotMsg(he));
    }
    if(success)
        d->bot_db->clearProcTable();
    return success;
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
QList<HistoryEntry> CuBotServer::m_prepareHistory(int uid, TBotMsgDecoder::Type t)
{
    QString type = TBotMsgDecoder().toHistoryTableType(t);
    QList<HistoryEntry> out = d->bot_db->history(uid, type);
    for(int i = 0; i < out.size(); i++) {
        BotReader *r = nullptr;
        HistoryEntry &he = out[i];
        if(t == TBotMsgDecoder::MonitorHistory  || t == TBotMsgDecoder::AlertHistory) {
            he.is_active = d->bot_mon && (r = d->bot_mon->findReaderByUid(he.user_id, he.name, he.host) ) != nullptr;
            if(r)
                he.index = r->index();
        }
    }
    return out;
}

void CuBotServer::m_removeExpiredProcs(QList<HistoryEntry> &in)
{
    d->botconf->ttl();
    QDateTime now = QDateTime::currentDateTime();
    QList<HistoryEntry >::iterator it = in.begin();
    while(it != in.end()) {
        if((*it).datetime.secsTo(now) >= d->botconf->ttl())
            it = in.erase(it);
        else
            ++it;
    }
}
