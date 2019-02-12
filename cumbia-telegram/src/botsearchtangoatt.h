#ifndef BOTSEARCHTANGOATT_H
#define BOTSEARCHTANGOATT_H

#include "volatileoperation.h"
#include <QStringList>
#include <QThread>

class TgAttSearchThread : public QThread
{
    Q_OBJECT
public:
    TgAttSearchThread(QObject *parent, const QString& devname);

    void run();

    QStringList attributes;

    QString errmsg;

private:
    QString m_devname;
};

class BotSearchTangoAtt : public QObject, public VolatileOperation
{
    Q_OBJECT
public:
    enum Type { AttSearch = 0x02 };

    BotSearchTangoAtt(QObject *parent, int chatid);

    virtual ~BotSearchTangoAtt();

    void find(const QString& devname);

    QString getAttByIdx(int idx);

    QString getSourceByIdx(int idx);

    QString devname() const;

    QStringList attributes() const;


signals:
    void attListReady(int chat_id, const QString& device, const QStringList& devs);

    void volatileOperationExpired(int chat_id, const QString& name, const QString& text);

private slots:
    void onSearchFinished();

private:
    QStringList m_attlist;
    QString m_devname;
    int m_chat_id;

    // VolatileOperation interface
public:
    void consume(TBotMsgDecoder::Type t);
    int type() const;
    QString name() const;
    void signalTtlExpired();
};

#endif // BOTSEARCHTANGOATT_H
