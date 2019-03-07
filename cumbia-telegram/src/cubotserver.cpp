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
#include "volatileoperations.h"
#include "botsearchtangodev.h"
#include "botsearchtangoatt.h"
#include "botcontrolserver.h"
#include "auth.h"
#include "botstats.h"
#include "botplotgenerator.h"
#include "cuformulaparsehelper.h"

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
#include <cuformulaplugininterface.h>
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
    VolatileOperations *volatile_ops;
    Auth* auth;
    BotControlServer *control_server;
    BotStats *stats;
};

CuBotServer::CuBotServer(QObject *parent) : QObject(parent)
{
    d = new CuBotServerPrivate;
    d->bot_listener = nullptr;
    d->bot_sender = nullptr;
    d->bot_mon = nullptr;
    d->botconf = nullptr;
    d->bot_db = nullptr;
    d->volatile_ops = nullptr;
    d->auth = nullptr;
    d->control_server = nullptr;
    d->stats = nullptr;
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
    if(success && !d->bot_db->userInPrivateChat(uid, m.chat_id)) {
        success = d->bot_db->addUserInPrivateChat(uid, m.chat_id);
    }

    TBotMsgDecoder msg_dec(m, d->cu_supervisor.formulaPlugin()->normalizedFormulaPattern());
    //    printf("type of message is %s [%d]\n", msg_dec.types[msg_dec.type()], msg_dec.type());
    TBotMsgDecoder::Type t = msg_dec.type();
    if(!d->auth->isAuthorized(m.user_id, t)) {
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().unauthorized(m.username, msg_dec.types[t],
                                                                          d->auth->reason()));
        fprintf(stderr, "\e[1;31;4mUNAUTH\e[0m: \e[1m%s\e[0m [uid %d] not authorized to exec \e[1;35m%s\e[0m: \e[3m\"%s\"\e[0m\n",
                qstoc(m.username), m.user_id, msg_dec.types[t], qstoc(d->auth->reason()));
    }
    else {
        //  user is authorized to perform operation type t
        //
        if(t == TBotMsgDecoder::Host) {
            QString host = msg_dec.host();
            QString new_host_description;
            success = d->bot_db->setHost(m.user_id, m.chat_id, host, new_host_description);
            if(success)
                success = d->bot_db->addToHistory(HistoryEntry(m.user_id, m.text, "host", "")); // "host" is type

            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().hostChanged(host, success, new_host_description), true); // silent
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
                lm.setHost(he.host);
                lm.text = he.toCommand();
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
            // inject host into src using CuFormulaParserHelper
            //    src = CuFormulaParseHelper().injectHost(host, src);
            BotReader *r = new BotReader(m.user_id, m.chat_id, this, d->cu_supervisor.cu_pool,
                                         d->cu_supervisor.ctrl_factory_pool, d->botconf->ttl(),
                                         msg_dec.text(), BotReader::High, host);
            connect(r, SIGNAL(newData(int, const CuData&)), this, SLOT(onReaderUpdate(int, const CuData& )));
            r->setPropertiesOnly(true); // only configure! no reads!
            r->setSource(src); // insert in  history db only upon successful connection
        }
        else if(t == TBotMsgDecoder::Monitor || t == TBotMsgDecoder::Alert) {
            QString src = msg_dec.source();
            if(!d->bot_mon)
                m_setupMonitor(); // insert in history db only upon successful connection
            QString host; // if m.hasHost use it, it comes from a fake history message created ad hoc by History
            m.hasHost() ? host = m.host() : host = d->bot_db->getSelectedHost(m.chat_id); // may be empty. If so, TANGO_HOST will be used
            // src = CuFormulaParseHelper().injectHost(host, src);
            // m.start_dt will be invalid if m is decoded by a real message
            // m.start_dt is forced to a given date and time when m is a fake msg built
            // from the database history
            success = d->bot_mon->startRequest(m.user_id, m.chat_id, d->auth->limit(), src, msg_dec.text(),
                                               t == TBotMsgDecoder::Monitor ? BotReader::Low : BotReader::High,
                                               host, m.start_dt);
            if(!success)
                d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("CuBotServer", d->bot_mon->message()));
        }
        else if(t == TBotMsgDecoder::StopMonitor && msg_dec.cmdLinkIdx() < 0) {
            QStringList srcs = CuFormulaParseHelper().sources(msg_dec.source());
            printf("\e[1;32m*** received StopMonitor cmd source is %s\e[0m\n", qstoc(srcs.join(", ")));
            printf("\e[1;32m*** received StopMonitor stop pattern is %s\e[0m\n", qstoc(msg_dec.getArgs().join(", ")));
            if(d->bot_mon) {
                success = d->bot_mon->stopAll(m.chat_id, srcs.isEmpty() ? msg_dec.getArgs() : srcs);
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
            BotSearchTangoDev *devSearch = new BotSearchTangoDev(this, m.chat_id);
            connect(devSearch, SIGNAL(devListReady(int, QStringList)), this, SLOT(onTgDevListSearchReady(int, QStringList)));
            connect(devSearch, SIGNAL(volatileOperationExpired(int,QString,QString)),
                    this, SLOT(onVolatileOperationExpired(int,QString,QString)));
            devSearch->find(msg_dec.source());
            d->volatile_ops->addOperation(m.chat_id, devSearch);
        }
        else if(t == TBotMsgDecoder::AttSearch) {
            int idx = msg_dec.cmdLinkIdx();
            QString devname;
            if(idx < 0) { // attlist  tango/dev/name
                devname = msg_dec.source(); // will contain tango device name
            }
            else {
                BotSearchTangoDev *sd = static_cast<BotSearchTangoDev *>(d->volatile_ops->get(m.chat_id, BotSearchTangoDev::DevSearch));
                if(sd) {
                    devname = sd->getDevByIdx(idx);
                }
            }
            if(!devname.isEmpty()) {
                BotSearchTangoAtt *sta = new BotSearchTangoAtt(this, m.chat_id);
                connect(sta, SIGNAL(attListReady(int, QString, QStringList)),
                        this, SLOT(onTgAttListSearchReady(int, QString, QStringList)));
                connect(sta, SIGNAL(volatileOperationExpired(int, QString,QString)),
                        this, SLOT(onVolatileOperationExpired(int, QString,QString)));
                sta->find(devname);
                d->volatile_ops->addOperation(m.chat_id, sta);
            }
            else {
                QStringList sequence = QStringList() << "search PATTERN" << QString("/attlist%1" ).arg(idx);
                d->bot_sender->sendMessage(m.chat_id,
                                           MsgFormatter().errorVolatileSequence(sequence));
            }
        }

        else if(t == TBotMsgDecoder::ReadFromAttList) {
            int idx = msg_dec.cmdLinkIdx();
            QString src;
            BotSearchTangoAtt* sta = static_cast<BotSearchTangoAtt *>(d->volatile_ops->get(m.chat_id, BotSearchTangoAtt::AttSearch));
            if(!sta) {
                QStringList sequence = QStringList() << "search PATTERN" << "/attlist{IDX}"
                                                     << QString("/a%1_read" ).arg(idx);
                d->bot_sender->sendMessage(m.chat_id,
                                           MsgFormatter().errorVolatileSequence(sequence));
            }
            else if(idx > 0 && (src = sta->getSourceByIdx(idx) ) != QString()) {
                TBotMsg mc = m; // copy m into mc
                mc.text = src;
                onMessageReceived(mc);
            }
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
        else if(t >= TBotMsgDecoder::Help && t <= TBotMsgDecoder::HelpSearch) {
            d->bot_sender->sendMessage(m.chat_id,
                                       MsgFormatter().help(t));
        }
        else if(t == TBotMsgDecoder::Start) {
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().help(TBotMsgDecoder::Help));
        }
        else if(t == TBotMsgDecoder::Plot) {
            printf("PLOT\n");
            BotPlotGenerator *plotgen =  static_cast<BotPlotGenerator *> (d->volatile_ops->get(m.chat_id, BotPlotGenerator::PlotGen));
            if(plotgen) {
                d->bot_sender->sendPic(m.chat_id, plotgen->generate());
            }
        }
        else if(t == TBotMsgDecoder::Invalid) {
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().error("TBotMsgDecoder", msg_dec.message()));
        }

        d->volatile_ops->consume(m.chat_id, msg_dec.type());
    } // else user is authorized
}

void CuBotServer::onReaderUpdate(int chat_id, const CuData &data)
{
    MsgFormatter mf;
    bool err = data["err"].toBool();
    d->stats->addRead(chat_id, data); // data is passed for error stats
    d->bot_sender->sendMessage(chat_id, mf.fromData(data));
    if(!err && m_isBigSizeVector(data)) {
        BotPlotGenerator *plotgen = new BotPlotGenerator(chat_id, data);
        d->volatile_ops->replaceOperation(chat_id, plotgen);
    }
    if(!err) {
        BotReader *reader = qobject_cast<BotReader *>(sender());
        HistoryEntry he(reader->userId(), reader->command(), "read", reader->host());
        d->bot_db->addToHistory(he);
    }
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
    if(m_isBigSizeVector(da)) {
        d->volatile_ops->replaceOperation(chat_id, new BotPlotGenerator(chat_id, da));
    }
    d->stats->addRead(chat_id, da); // da is passed for error stats
}

void CuBotServer::onSrcMonitorStopped(int user_id, int chat_id, const QString &src,
                                      const QString &host, const QString &message)
{
    const bool silent = true;
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    d->bot_sender->sendMessage(chat_id,  MsgFormatter().monitorStopped(r->command(), message), silent);
    // update database, remove rows for chat_id and src
    d->bot_db->monitorStopped(chat_id, src);
}

void CuBotServer::onSrcMonitorStarted(int user_id, int chat_id, const QString &src,
                                      const QString& host, const QString& formula)
{
    const QDateTime until = QDateTime::currentDateTime().addSecs(d->botconf->ttl());
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    BotReader::Priority pri = r->priority();
    d->bot_sender->sendMessage(chat_id, MsgFormatter().monitorUntil(r->command(), until));
    // record new monitor into the database
    qDebug() << __PRETTY_FUNCTION__ << "adding history entry with formula " << formula << "host " << r->host();
    HistoryEntry he(user_id, r->command(), pri == BotReader::High ? "alert" :  "monitor", host);
    d->bot_db->addToHistory(he);
}

void CuBotServer::onSrcMonitorStartError(int chat_id, const QString &src, const QString &message)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().srcMonitorStartError(src, message));
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
            d->bot_listener = new CuBotListener(this,
                                                d->botconf->getBotListenerMsgPollMillis(),
                                                d->botconf->getBotListenerOldMsgDiscardSecs());
            connect(d->bot_listener, SIGNAL(onNewMessage(const TBotMsg &)),
                    this, SLOT(onMessageReceived(const TBotMsg&)));
            d->bot_listener->start();
        }
        if(!d->bot_sender) {
            d->bot_sender = new CuBotSender(this);
        }
        if(!d->volatile_ops)
            d->volatile_ops = new VolatileOperations();

        if(!d->auth)
            d->auth = new Auth(d->bot_db, d->botconf);

        d->control_server = new BotControlServer(this);
        connect(d->control_server, SIGNAL(newMessage(int, int, ControlMsg::Type, QString, QLocalSocket*)),
                this, SLOT(onNewControlServerData(int, int, ControlMsg::Type, QString, QLocalSocket*)));

        if(!d->stats)
            d->stats = new BotStats(this);

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
            d->bot_db = nullptr;
        }
        if(d->bot_mon) {
            foreach(BotReader *r, d->bot_mon->readers())
                delete r;
        }
        d->cu_supervisor.dispose();

        if(d->volatile_ops) {
            delete d->volatile_ops;
            d->volatile_ops = nullptr;
        }
        if(d->auth){
            delete d->auth;
            d->auth = nullptr;
        }
        if(d->control_server)
            delete d->control_server;

        if(d->stats) {
            delete d->stats;
            d->stats = nullptr;
        }
    }
}

void CuBotServer::onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &new_s, const QString& host,
                                             const QString &old, const QString &new_f)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().formulaChanged(new_s, old, new_f));
    BotReader *r = d->bot_mon->findReader(chat_id, new_s, host);
    printf("\e[1;33mADD TO HISTORY NEW ENTRY: %s\e[0m\n", qstoc(new_s));
    HistoryEntry he(user_id, new_f, r->priority() == BotReader::Low ? "monitor" : "alert", r->host());
    d->bot_db->addToHistory(he);
}

void CuBotServer::onSrcMonitorTypeChanged(int user_id, int chat_id, const QString &src,
                                          const QString& host, const QString &old_type, const QString &new_type)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().monitorTypeChanged(src, old_type, new_type));
    BotReader *r = d->bot_mon->findReader(chat_id, src, host);
    HistoryEntry he(user_id, r->command(), new_type,  r->host());
    d->bot_db->addToHistory(he);
}

void CuBotServer::onTgDevListSearchReady(int chat_id, const QStringList &devs)
{
    qDebug() << __PRETTY_FUNCTION__ << chat_id << devs;
    d->bot_sender->sendMessage(chat_id, MsgFormatter().tg_devSearchList(devs));
    d->stats->addRead(chat_id, CuData("err", false)); // CuData is passed for error stats
}

void CuBotServer::onTgAttListSearchReady(int chat_id, const QString& devname, const QStringList &atts)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().tg_attSearchList(devname, atts));
    d->stats->addRead(chat_id, CuData("err", false)); // CuData is passed for error stats
}

void CuBotServer::onVolatileOperationExpired(int chat_id, const QString &opnam, const QString &text)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().volatileOpExpired(opnam, text));
}

void CuBotServer::onNewControlServerData(int uid, int chat_id, ControlMsg::Type t, const QString &msg, QLocalSocket *so)
{
    qDebug() << __PRETTY_FUNCTION__ << uid << chat_id << t << msg;
    if(t == ControlMsg::Statistics) {
        QString stats = BotStatsFormatter().toJson(d->stats, d->bot_db, d->bot_mon);
        d->control_server->sendControlMessage(so, stats);
    }
    else if(chat_id > -1 && d->bot_sender) {
        d->bot_sender->sendMessage(chat_id, MsgFormatter().fromControlData(t, msg));
    }
    else if(uid > -1 && chat_id < 0) {
        foreach(int chat_id, d->bot_db->chatsForUser(uid)) {
            d->bot_sender->sendMessage(chat_id, MsgFormatter().fromControlData(t, msg));
        }
    }
}

void CuBotServer::m_setupMonitor()
{
    if(!d->bot_mon) {
        d->bot_mon = new BotMonitor(this, d->cu_supervisor.cu_pool, d->cu_supervisor.ctrl_factory_pool, d->botconf->ttl());
        connect(d->bot_mon, SIGNAL(newMonitorData(int, const CuData&)),
                this, SLOT(onNewMonitorData(int, const CuData&)));
        connect(d->bot_mon, SIGNAL(stopped(int, int, QString, QString, QString)),
                this, SLOT(onSrcMonitorStopped(int, int, QString, QString, QString)));
        connect(d->bot_mon, SIGNAL(started(int,int, QString,QString,QString)), this, SLOT(onSrcMonitorStarted(int,int, QString,QString,QString)));
        connect(d->bot_mon, SIGNAL(onFormulaChanged(int, int, QString,QString,QString,QString)),
                this, SLOT(onSrcMonitorFormulaChanged(int, int, QString,QString,QString,QString)));
        connect(d->bot_mon, SIGNAL(onMonitorTypeChanged(int,int, QString,QString,QString,QString)),
                this, SLOT(onSrcMonitorTypeChanged(int,int, QString,QString,QString,QString)));
        connect(d->bot_mon, SIGNAL(startError(int, const QString&, const QString&)), this,
                SLOT(onSrcMonitorStartError(int, const QString&, const QString&)));
    }
}

bool CuBotServer::m_saveProcs()
{
    if(d->bot_mon) {
        foreach(BotReader *r, d->bot_mon->readers()) {
            HistoryEntry he(r->userId(), r->command(),
                            r->priority() == BotReader::High ? "alert" :  "monitor",
                            r->host());
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
    if(!d->bot_mon && out.size() > 0)
        m_setupMonitor();
    for(int i = 0; i < out.size(); i++) {
        BotReader *r = nullptr;
        HistoryEntry &he = out[i];
        if(t == TBotMsgDecoder::MonitorHistory  || t == TBotMsgDecoder::AlertHistory) {
            r = d->bot_mon->findReaderByUid(he.user_id, he.command, he.host);
            he.is_active = (r != nullptr && he.command == r->command());
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

bool CuBotServer::m_isBigSizeVector(const CuData &da) const
{
    if(da.containsKey("value") && da.containsKey("data_format_str")
            && da["data_format_str"].toString() == std::string("vector")) {
        const CuVariant& val = da["value"];
        return val.getSize() > 5;
    }
    return false;
}
