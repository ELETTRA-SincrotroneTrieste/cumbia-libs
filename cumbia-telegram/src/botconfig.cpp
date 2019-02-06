#include "botconfig.h"

class BotConfigPrivate {
public:
    int ttl;
};

BotConfig::BotConfig()
{
    d = new BotConfigPrivate;
     d->ttl = 24 * 3600;
}

int BotConfig::ttl() const
{
    return d->ttl;
}
