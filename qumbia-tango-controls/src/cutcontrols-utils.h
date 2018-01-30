#ifndef CUTCONTROLSUTILS_H
#define CUTCONTROLSUTILS_H

#include <QStringList>

class CuTControlsUtils
{
public:
    CuTControlsUtils();

    QString replaceWildcards(const QString &s, const QStringList& args);
};

#endif // CUTCONTROLSUTILS_H
