#ifndef CUCONTROLSUTILS_H
#define CUCONTROLSUTILS_H

#include <cuvariant.h> // defines also TIMESTAMPLEN used here
#include <QList>

#define MSGLEN 256

class QString;
class QObject;
class CuInputProvider_I;
class CuData;

class CuControlsUtils_P;

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
    CuControlsUtils();
    virtual ~CuControlsUtils();
    QString findInput(const QString& objectName, const QObject* leaf, bool *found) const;
    QObject *findObject(const QString& objectName, const QObject* leaf) const;
    CuVariant getArgs(const QString& target, const QObject *leaf) const;
    QList<QObject *> findObjects(const QString &target, const QObject *leaf);
    bool initObjects(const QString& target, const QObject* leaf, const CuData &data, const char *value_key);
    QString msg(const CuData& da) const;
    void msg_short(const CuData& da, char buf[MSGLEN]);

    inline void ts_to_s(const long int &millis, char dt[TIMESTAMPLEN]) const; // TIMESTAMPLEN from cuvariant.h
    inline void ts_to_s(const double &ts, char dt[TIMESTAMPLEN]) const; //

    inline void m_elapsed(time_t secs, const long int& millis, char buf[MSGLEN], int offset) const;
    inline void m_elapsed(const double &us, char buf[MSGLEN], int offset) const;

private:
    CuControlsUtils_P *d;
};

#endif // CUCONTROLSUTILS_H
