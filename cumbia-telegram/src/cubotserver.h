#ifndef CUBOTSERVER_H
#define CUBOTSERVER_H

#include <QObject>
#include <tbotmsg.h>

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

public slots:
    void start();
    void stop();

private:
    CuBotServerPrivate *d;

    void setupCumbia();
    void disposeCumbia();
};

#endif // CUBOTSERVER_H
