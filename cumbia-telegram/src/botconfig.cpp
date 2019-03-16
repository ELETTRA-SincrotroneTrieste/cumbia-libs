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
    d->map["poll_period"] = 15000;
    d->map["max_avg_poll_period"] = 1000;
    d->map["history_depth"] = 12;

    QStringList default_operations_auth = QStringList() << "monitor" << "reads" << "host" << "dbsearch";
    d->map["default_monitors_auth"] = 10;
    d->map["default_reads_auth"] = 1;
    d->map["default_host_auth"] = 1;
    d->map["default_dbsearch_auth"] = 1;

    d->map["botlistener_msg_poll_ms"] = 1000;
    d->map["botlistener_discard_msg_older_than_secs"] = 30;

    d->map["max_alias_cnt"] = 12;

    // temporary dir for images (Plot)
    d->map["img_out_tmpdir"] = "/tmp/cumbia-telegram/botplot/";

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

int BotConfig::poll_period() const
{
    return d->map["poll_period"].toInt();
}

int BotConfig::max_avg_poll_period() const
{
    return d->map["max_avg_poll_period"].toInt();
}

int BotConfig::getHistoryDepth() const
{
    return d->map["history_depth"].toInt();
}

int BotConfig::getDefaultAuth(const QString &operation) const
{
    QString key = QString("default_%1_auth").arg(operation);
    if(d->map.contains(key))
        return d->map[key].toInt();
    perr("BotConfig.getDefaultAuth: operation \"%s\" is not valid", qstoc(operation));
    return -1;
}

int BotConfig::getBotListenerMsgPollMillis() const
{
    return d->map["botlistener_msg_poll_ms"].toInt();
}

int BotConfig::getBotListenerOldMsgDiscardSecs() const
{
    return d->map["botlistener_discard_msg_older_than_secs"].toInt();
}

bool BotConfig::isValid() const
{
    return d->valid;
}

QString BotConfig::error() const
{
    return d->error;
}

QString BotConfig::getString(const QString &key) const
{
    return  d->map[key].toString();
}

int BotConfig::getInt(const QString &key) const
{
    return  d->map[key].toInt();
}
