#include "botsearchtangodev.h"
#include <tango.h>
#include <QtDebug>

BotSearchTangoDev::BotSearchTangoDev(QObject *parent, int chatid) : QObject (parent)
{
    m_chat_id = chatid;
}

BotSearchTangoDev::~BotSearchTangoDev()
{
    TgDevSearchThread *th = findChild<TgDevSearchThread *>();
    if(th)
        th->wait();
    printf("\e[1;31mx BotSearchTangoDev %p\e[0m\n", this);
}

void BotSearchTangoDev::find(const QString &pattern)
{
    m_pattern = pattern;
    TgDevSearchThread *th = new TgDevSearchThread(this, pattern);
    connect(th, SIGNAL(finished()), this, SLOT(onSearchFinished()));
    th->start();
}

QString BotSearchTangoDev::getDevByIdx(int idx)
{
    if(idx > 0 && idx - 1 < m_devlist.size())
        return m_devlist.at(idx - 1);
    d_error = true;
    d_msg = QString("BotSearchTangoDev.getDevByIdx: index %1 out of range").arg(idx);
    return QString();
}

QStringList BotSearchTangoDev::devices() const
{
    return m_devlist;
}

void BotSearchTangoDev::signalTtlExpired()
{
    emit volatileOperationExpired(m_chat_id, name(), "search " + m_pattern);
}

void BotSearchTangoDev::onSearchFinished()
{
    m_devlist = qobject_cast<TgDevSearchThread *>(sender())->devices;
    sender()->deleteLater();
    emit devListReady(m_chat_id, m_devlist);
}

TgDevSearchThread::TgDevSearchThread(QObject *parent, const QString &pattern) : QThread(parent)
{
    m_pattern = pattern;
}

void TgDevSearchThread::run()
{
    Tango::Database db;
    devices.clear();
    m_pattern.replace("//", "/*/");
    std::string pattern = m_pattern.toStdString();
    Tango::DbDatum dbd  = db.get_device_exported(pattern);
    std::vector<std::string> devs;
    dbd >> devs;
    for(size_t i = 0; i < devs.size(); i++)
        devices << QString::fromStdString(devs[i]);
}

/**
 * @brief BotSearchTangoDev::consume consume the life of this object if the input type is not AttSearch
 *
 * @param t the type of message that causes this VolatileOperation to consume itself or not
 */
void BotSearchTangoDev::consume(TBotMsgDecoder::Type t)
{
    if(t != TBotMsgDecoder::AttSearch)
        d_life_cnt--;
}

int BotSearchTangoDev::type() const
{
    return DevSearch;
}

QString BotSearchTangoDev::name() const
{
    return "Tango device search";
}
