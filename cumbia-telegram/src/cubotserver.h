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

signals:

private slots:
    void onMessageReceived(const TBotMsg &m);

    void onNewData(int chat_id, const CuData& d);

    void onNewMonitorData(int chat_id, const CuData& da);

    void onSrcMonitorStopped(int chat_id, const QString& src, const QString& message);

    void onSrcMonitorStarted(int user_id, int chat_id, const QString& src, const QString &formula);

    void onSrcMonitorAlarm(int chat_id, const CuData& da);

    void onSrcMonitorFormulaChanged(int user_id, int chat_id, const QString &src, const QString &old, const QString &new_f);

public slots:
    void start();
    void stop();

private:
    CuBotServerPrivate *d;

    void setupCumbia();
    void disposeCumbia();

    void m_setupMonitor();

    QList<HistoryEntry> m_prepareHistory(const QList<HistoryEntry>& in, TBotMsgDecoder::Type t);

};

#endif // CUBOTSERVER_H
