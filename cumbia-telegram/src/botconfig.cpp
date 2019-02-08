#include "botconfig.h"
#include "botdb.h"
#include <cumacros.h>
#include <QString>
#include <QMap>
#include <QVariant>

class BotConfigPrivate {
public:
    QMap<QString, QVariant> map;
    QMap<QString, QString> descmap;
    bool valid;
    QString error;
};

/**
 * @brief BotConfig::BotConfig
 * Build a default BotConfig
 */
BotConfig::BotConfig(BotDb *db)
{
    d = new BotConfigPrivate;
    // default value for TTL: one day
    // getConfig does not clear d->map, so that keys that are not configured into the
    // database will remain with their defaults
    d->map["ttl"] = 24 * 3600;
    d->map["history_depth"] = 8;
    d->valid = db->getConfig(d->map, d->descmap);
    if(!d->valid) {
        d->error = "BotConfig: database reported an error: " + db->message();
        perr("%s", qstoc(d->error));
    }
}

int BotConfig::ttl() const
{
    return d->map["ttl"].toInt();
}

int BotConfig::getHistoryDepth() const
{
    return d->map["history_depth"].toInt();
}

bool BotConfig::isValid() const
{
    return d->valid;
}

QString BotConfig::error() const
{
    return d->error;
}
