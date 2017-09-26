#ifndef CUTCONTROLSUTILS_H
#define CUTCONTROLSUTILS_H

#include <QStringList>

class CuEpControlsUtils
{
public:
    CuEpControlsUtils();

    QString replaceWildcards(const QString &s, const QStringList& args);
};

#endif // CUTCONTROLSUTILS_H
