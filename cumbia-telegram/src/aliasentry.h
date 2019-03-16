#ifndef ALIASENTRY_H
#define ALIASENTRY_H

#include <QStringList>

class AliasEntry
{
public:
    AliasEntry(const QString& nam, const QString& repl, const QString& descrip);
    AliasEntry();

    QString name, replaces, description;

    bool in_history;
    QStringList in_history_hosts;
    QList<int> in_history_idxs;
};

#endif // ALIASENTRY_H
