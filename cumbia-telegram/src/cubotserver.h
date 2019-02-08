#ifndef CUBOTSERVER_H
#define CUBOTSERVER_H

#include <QObject>
#include <tbotmsg.h>
#include <tbotmsgdecoder.h>
#include <botdb.h>

class CuBotServerPrivate;
class QJsonValue;
class CuData;

class CuBotServer : public QObject
{
    Q_OBJECT
public:
    explicit CuBotServer(QObject *parent = nullptr);

    virtual ~CuBotServer();

    bool isRunning() const;

signals:

private slots:
    void onMessageReceived(const TBotMsg &m);

    void onNewData(int chat_id, const CuData& d);

    void onNewMonitorData(int chat_id, const CuData& da);

    void onSrcMonitorStopped(int user_id, int chat_id, const QString& src, const QString& host, const QString& message);

    void onSrcMonitorStarted(int user_id, int chat_id, const QString& src, const QString &host, const QString &formula);

    void onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &src,
                                    const QString &host, const QString &old, const QString &new_f);

    void onSrcMonitorTypeChanged(int user_id, int chat_id, const QString& src,
                                 const QString& host, const QString& old_type, const QString& new_type);

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

    QList<HistoryEntry> m_prepareHistory(int uid, TBotMsgDecoder::Type t);
    void m_removeExpiredProcs(QList<HistoryEntry> &in);
};

#endif // CUBOTSERVER_H
