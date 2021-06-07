#ifndef CUCONTROLSUTILS_H
#define CUCONTROLSUTILS_H

#include <cuvariant.h>
#include <QList>

class QString;
class QObject;
class CuInputProvider_I;
class CuData;

/*! \brief utility class to find input arguments from objects with text or value properties
 *
 * This class is used to search input arguments for sources or targets from
 * widgets exposing text or value properties. Text or values are edited within the
 * widget and can be passed as input arguments to readers or writers.
 *
 * Please refer to the findInput method documentation for more details.
 *
 */
class CuControlsUtils
{
public:
    QString findInput(const QString& objectName, const QObject* leaf) const;
    QObject *findObject(const QString& objectName, const QObject* leaf) const;
    CuVariant getArgs(const QString& target, const QObject *leaf) const;
    QList<QObject *> findObjects(const QString &target, const QObject *leaf);
    bool initObjects(const QString& target, const QObject* leaf, const CuData &data, const char *value_key);
    QString msg(const CuData& da, const QString& date_time_fmt = "yyyy-MM-dd HH:mm:ss.zzz") const;
};

#endif // CUCONTROLSUTILS_H
