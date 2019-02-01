#include "cubotserver.h"
#include "cubotlistener.h"
#include "cubotsender.h"
#include "tbotmsg.h"
#include "tbotmsgdecoder.h"
#include "botdb.h"
#include "botreader.h"
#include "cumbiasupervisor.h"
#include "msgformatter.h"

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
    CumbiaSupervisor cu_supervisor;
};

CuBotServer::CuBotServer(QObject *parent) : QObject(parent)
{
    d = new CuBotServerPrivate;
    d->bot_listener = nullptr;
    d->bot_sender = nullptr;
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
    printf("type of message is %s [%d]\n", msg_dec.types[msg_dec.type()], msg_dec.type());
    TBotMsgDecoder::Type t = msg_dec.type();
    if(t == TBotMsgDecoder::Host) {
        QString host = msg_dec.host();
        success = d->bot_db->setHost(m.user_id, host);
        success = d->bot_db->insertOperation(m.user_id, m.text);
        if(!success)
            perr("CuBotServer::onMessageReceived: database error: %s", qstoc(d->bot_db->message()));
    }
    else if(t == TBotMsgDecoder::Last) {
        QString last_op, timestamp;
        QDateTime dt;
        bool has_last = d->bot_db->lastOperation(m.user_id, last_op, dt);
        if(has_last) {
            TBotMsg lm = m;
            lm.text = last_op;
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().lastOperation(dt, last_op));
            //
            // call ourselves with the updated copy of the received message
            //
            onMessageReceived(lm);
        }
    }
    else if(t == TBotMsgDecoder::Read) {
        QString src = msg_dec.source();
        BotReader *r = new BotReader(m.chat_id, this, d->cu_supervisor.cu_pool, d->cu_supervisor.ctrl_factory_pool);
        connect(r, SIGNAL(newData(int, const CuData&)), this, SLOT(onNewData(int, const CuData& )));
        r->setPropertyEnabled(false);
        r->setSource(src);
        d->bot_db->insertOperation(m.user_id, m.text);
    }
    else if(t == TBotMsgDecoder::History) {
        QStringList shortcut_cmds, timestamps, cmds;
        bool ok = d->bot_db->history(m.user_id, shortcut_cmds, timestamps, cmds);
        d->bot_sender->sendMessage(m.chat_id, MsgFormatter().history(shortcut_cmds, timestamps, cmds));
    }
    else if(t == TBotMsgDecoder::CmdLink) {
        int cmd_idx = msg_dec.cmdLinkIdx();
        if(cmd_idx > -1) {
            QDateTime dt;
            QString operation;
            success = d->bot_db->commandFromIndex(m.user_id, cmd_idx, dt, operation);
            // 1. remind the user what was the command linked to /commandN
            d->bot_sender->sendMessage(m.chat_id, MsgFormatter().lastOperation(dt, operation));
            // 2.
            // call ourselves with the updated copy of the received message
            //
            TBotMsg lnkm = m;
            lnkm.text = operation;
            onMessageReceived(lnkm);
        }
    }

}

void CuBotServer::onNewData(int chat_id, const CuData &data)
{
    d->bot_sender->sendMessage(chat_id, MsgFormatter().fromData(data));
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
