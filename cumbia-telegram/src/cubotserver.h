#ifndef CUBOTSERVER_H
#define CUBOTSERVER_H

#include <QObject>
#include <tbotmsg.h>
#include <tbotmsgdecoder.h>
#include <botdb.h>
#include <botreader.h> // for BotReader::RefreshMode

#include "../cumbia-telegram-defs.h" // for ControlMsg::Type

class CuBotServerPrivate;
class QJsonValue;
class CuData;
class QLocalSocket;

class CuBotServer : public QObject
{
    Q_OBJECT
public:
    explicit CuBotServer(QObject *parent, const QString &bot_key, const QString& sqlite_db_filenam);

    virtual ~CuBotServer();

    bool isRunning() const;

signals:

private slots:
    void onMessageReceived(TBotMsg &m);

    void onReaderUpdate(int chat_id, const CuData& d);

    void onNewMonitorData(int chat_id, const CuData& da);

    void onSrcMonitorStopped(int user_id, int chat_id, const QString& src, const QString& host, const QString& message);

    void onSrcMonitorStarted(int user_id, int chat_id, const QString& src, const QString &host, const QString &formula);

    void onSrcMonitorStartError(int chat_id, const QString& src, const QString& message);

    void onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &new_s,
                                    const QString &host, const QString &old, const QString &new_f);

    void onSrcMonitorTypeChanged(int user_id, int chat_id, const QString& src,
                                 const QString& host, const QString& old_type, const QString& new_type);

    void onTgDevListSearchReady(int chat_id , const QStringList& devs);

    void onTgAttListSearchReady(int chat_id, const QString& devname, const QStringList& atts);

    void onVolatileOperationExpired(int chat_id, const QString& opnam, const QString& text);


    // control server data
    void onNewControlServerData(int uid, int chat_id, ControlMsg::Type t, const QString& msg, QLocalSocket *so);

    void m_onReaderRefreshModeChanged(int user_id, int chat_id, const QString& src, const QString& host, BotReader::RefreshMode rm);


public slots:
    void start();
    void stop();

private:
    CuBotServerPrivate *d;

    void setupCumbia();
    void disposeCumbia();

    void m_setupMonitor();

    bool m_saveProcs();

    bool m_restoreProcs();

    bool m_broadcastShutdown();

    QList<HistoryEntry> m_prepareHistory(int uid, TBotMsgDecoder::Type t);
    void m_removeExpiredProcs(QList<HistoryEntry> &in);

    bool m_isBigSizeVector(const CuData &da) const;

    QString m_getHost(int chat_id, const QString& src = QString());

};

#endif // CUBOTSERVER_H
