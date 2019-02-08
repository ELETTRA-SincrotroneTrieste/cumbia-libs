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

    int getHistoryDepth() const;

    bool isValid() const;

    QString error() const;

private:
    BotConfigPrivate *d;
};

#endif // CONFIG_H
