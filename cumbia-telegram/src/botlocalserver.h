#ifndef BOTLOCALSERVER_H
#define BOTLOCALSERVER_H

#include <QObject>


class BotLocalServerPrivate;

class BotLocalServer : public QObject
{
    Q_OBJECT
public:
    BotLocalServer(QObject *parent);

    ~BotLocalServer();

    bool error() const;

    QString message() const;

private slots:
    void onNewConnection();
    void onNewData();

private:
    BotLocalServerPrivate *d;
};

#endif // BOTLOCALSERVER_H
