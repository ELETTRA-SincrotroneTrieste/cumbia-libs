#ifndef BOTCONFIG_H
#define BOTCONFIG_H

class BotConfigPrivate;

#include <QMap>
#include <QVariant>

class BotDb;

class BotConfig
{
public:
    BotConfig(BotDb *db);

    int ttl() const;

    int poll_period() const;

    int max_avg_poll_period() const;

    int getHistoryDepth() const;

    int getDefaultAuth(const QString& operation) const;

    int getBotListenerMsgPollMillis() const;

    int getBotListenerOldMsgDiscardSecs() const;

    bool isValid() const;

    QString error() const;

    QString getString(const QString& key) const;

    int getInt(const QString &key) const;

private:
    BotConfigPrivate *d;
};

#endif // CONFIG_H
