#include "botsearchtangoatt.h"
#include <tango.h>
#include <cutango-world.h>

BotSearchTangoAtt::BotSearchTangoAtt(QObject *parent, int chatid) : QObject (parent)
{
    m_chat_id = chatid;
}

BotSearchTangoAtt::~BotSearchTangoAtt()
{
    printf("\e[1;31mx BotSearchTangoAtt %p\e[0m\n", this);
    TgAttSearchThread *th = findChild<TgAttSearchThread *>();
    if(th)
        th->wait();
}

void BotSearchTangoAtt::find(const QString &devname)
{
    m_devname = devname;
    TgAttSearchThread *th = new TgAttSearchThread(this, devname);
    connect(th, SIGNAL(finished()), this, SLOT(onSearchFinished()));
    th->start();
}

QString BotSearchTangoAtt::getSourceByIdx(int idx)
{
    QString a = getAttByIdx(idx);
    if(a.length() > 0)
        return m_devname + "/" + a;
    return a;
}

QString BotSearchTangoAtt::getAttByIdx(int idx)
{
    if(idx > 0 && idx - 1 < m_attlist.size())
        return m_attlist.at(idx - 1);
    d_error = true;
    d_msg = QString("BotSearchTangoAtt.getDevByIdx: index %1 out of range").arg(idx);
    return QString();
}

QString BotSearchTangoAtt::devname() const
{
    return m_devname;
}

QStringList BotSearchTangoAtt::attributes() const
{
    return m_attlist;
}

void BotSearchTangoAtt::onSearchFinished()
{
    m_attlist = qobject_cast<TgAttSearchThread *>(sender())->attributes;
    sender()->deleteLater();
    emit attListReady(m_chat_id, m_devname, m_attlist);
}

void BotSearchTangoAtt::signalTtlExpired()
{
    emit volatileOperationExpired(m_chat_id, name(), "attlist " + m_devname);
}

QString BotSearchTangoAtt::name() const
{
    return "search tango device attributes";
}

TgAttSearchThread::TgAttSearchThread(QObject *parent, const QString &devname) : QThread(parent)
{
    m_devname = devname;
}

void TgAttSearchThread::run()
{
    errmsg.clear();
    try {
        Tango::DeviceProxy *dev = new Tango::DeviceProxy(qstoc(m_devname));
        Tango::AttributeInfoList *ail = dev->attribute_list_query();
        for(size_t i = 0; i < ail->size(); i++)
            attributes << QString::fromStdString(ail->at(i).name);
    }
    catch(Tango::DevFailed &e) {
        CuTangoWorld tw;
        errmsg = QString::fromStdString(tw.strerror(e));
    }
}

/**
 * @brief BotSearchTangoAtt::consume consume the life of this object if the input type is not AttSearch
 *
 * @param t the type of message that causes this VolatileOperation to consume itself or not
 */
void BotSearchTangoAtt::consume(TBotMsgDecoder::Type t)
{
    if(t != TBotMsgDecoder::ReadFromAttList && t != TBotMsgDecoder::Read) {
        d_life_cnt--;
    }
}

int BotSearchTangoAtt::type() const
{
    return AttSearch;
}
