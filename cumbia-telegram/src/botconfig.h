#ifndef BOTCONFIG_H
#define BOTCONFIG_H

class BotConfigPrivate;

class BotConfig
{
public:
    BotConfig();

    int ttl() const;

private:
    BotConfigPrivate *d;
};

#endif // CONFIG_H
