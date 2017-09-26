#ifndef CUEPCONTROLSUTILS_H
#define CUEPCONTROLSUTILS_H

#include <QStringList>

class CuEpControlsUtils
{
public:
    CuEpControlsUtils();

    QString replaceWildcards(const QString &s, const QStringList& args);
};

#endif // CUEPCONTROLSUTILS_H
