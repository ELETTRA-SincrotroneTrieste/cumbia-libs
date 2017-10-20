#include "conf.h"

Conf::Conf(const QString &fac)
{
    factory = fac;
}

void Conf::add(const QString &param)
{
    adds.append(param);
}

void Conf::add(const QString &orig, const QString &_new)
{
    subs.insert(orig, _new);
}
