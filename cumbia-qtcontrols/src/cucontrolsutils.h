#ifndef CUCONTROLSUTILS_H
#define CUCONTROLSUTILS_H

#include <cuvariant.h>

class QString;
class QObject;
class CuInputProvider_I;

class CuControlsUtils
{
public:
    CuControlsUtils();

    QString findInput(const QString& objectName, const QObject* leaf) const;

    CuVariant getArgs(const QString& target, const QObject *leaf) const;
};

#endif // CUCONTROLSUTILS_H
