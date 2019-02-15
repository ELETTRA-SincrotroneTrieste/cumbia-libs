#ifndef BOTCONTROLSERVER_H
#define BOTCONTROLSERVER_H

#include <QObject>

#include "../cumbia-telegram-defs.h" // LOCALSERVER_NAME

class BotLocalServerPrivate;

class BotControlServer : public QObject
{
    Q_OBJECT
public:
    BotControlServer(QObject *parent);

    ~BotControlServer();

    bool error() const;

    QString message() const;

signals:
    void newMessage(int uid, int chat_id, ControlMsg::Type t, const QString& msg);

private slots:
    void onNewConnection();
    void onNewData();

private:
    BotLocalServerPrivate *d;
};

#endif // BOTLOCALSERVER_H
