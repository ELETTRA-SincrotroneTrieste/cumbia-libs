#include "aliasentry.h"

AliasEntry::AliasEntry()
{
    in_history = false;
}

AliasEntry::AliasEntry(const QString &nam, const QString &repl, const QString &descrip)
{
     description = descrip;
     replaces = repl;
     name = nam;
     in_history = false;
}
