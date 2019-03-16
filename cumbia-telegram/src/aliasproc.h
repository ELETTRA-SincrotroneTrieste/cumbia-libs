#ifndef ALIASPROC_H
#define ALIASPROC_H

#include <QList>
#include "aliasentry.h"

class AliasProc
{
public:
    AliasProc();
    QString findAndReplace(const QString& in, const QList<AliasEntry>& aliases);
};

#endif // ALIASPROC_H
