#ifndef AUTH_H
#define AUTH_H

class BotDb;
class BotConfig;

#include <tbotmsgdecoder.h>

class Auth
{
public:
    Auth(BotDb *db, BotConfig *cfg);

    bool isAuthorized(int uid, TBotMsgDecoder::Type t);

    int limit() const;

    QString reason() const;

private:
    BotDb *m_db;
    BotConfig *m_cfg;
    int m_limit;
    QString m_reason;
};

#endif // AUTH_H
