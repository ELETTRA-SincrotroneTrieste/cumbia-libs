#ifndef VOLATILEOPERATION_H
#define VOLATILEOPERATION_H

#include <tbotmsgdecoder.h>
#include <QDateTime>

class VolatileOperation
{
public:
    VolatileOperation();

    virtual ~VolatileOperation();

    virtual void consume(TBotMsgDecoder::Type t) = 0;

    virtual int lifeCount() const;

    virtual int type() const = 0;

    virtual QString name() const = 0;

    virtual void signalTtlExpired() = 0;

    // default time to live of a volatile operation, in seconds
    virtual int ttl() const;

    QDateTime creationTime();

    QString message() const;

    bool error() const;

protected:
    int d_life_cnt;

    QString d_msg;
    bool d_error;

private:
    QDateTime m_datetime;
};

#endif // VOLATILEOPERATION_H
