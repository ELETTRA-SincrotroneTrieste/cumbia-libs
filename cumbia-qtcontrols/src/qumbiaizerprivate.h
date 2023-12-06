#ifndef QUMBIAIZERPRIVATE_H
#define QUMBIAIZERPRIVATE_H

#include <QString>
#include <QHash>
#include <cucontrolsutils.h>
#include "qumbiaizer.h"

class QObject;
class QuValueFilter;
class CuData;
class CuVariant;

#define TYPELEN 64

/*! @private */
class QumbiaizerPrivate
{
    Q_DECLARE_PUBLIC(Qumbiaizer)
public:

    QumbiaizerPrivate();

    virtual ~QumbiaizerPrivate();

    /* type of object, for rtti */
    int type;

    QString slot, methodName;

    QString setPointSlot, setPointMethodName;

    void *data;

    Qt::ConnectionType connType;

    QHash<int, QString> autoConfSlotsHash;

    bool inTypeOfMethod(const QString& method, QObject *obj, char* in_type);

    bool configure(const CuData &da, QObject *obj);

    bool singleShot, autoDestroy;

    bool toolTipsDisabled;

    void setExecuteOnConnection(bool execute, const CuVariant &argument);

    CuVariant executeArgument() const { return m_executeArgument; }

    bool executeOnConnection() const { return m_executeOnConnection; }

    /* methods are specified via SIGNAL and SLOT macros, so we have to remove
     * signals and slots codes from the method. See qt/src/corelib/kernel/qobject.h
     *  and qt/src/corelib/kernel/qobject.cpp.
     * Returns the extracted code and removes the code from the method parameter if
     * the code is QSIGNAL_CODE or QSLOT_CODE.
     */
    int extractCode(QString& method);

    QuValueFilter *refreshFilter;

    bool error;
    QString message;
    void setError(bool err);
    CuControlsUtils cucu;
    CuData last_d;
    char msg[MSGLEN];

private:
    Qumbiaizer *q_ptr;

    CuVariant m_executeArgument;

    bool m_executeOnConnection;

    void *m_getDataPtr(const char *type, double val);
    void m_DeleteDataPtr(const char *type, void *dataptr);
};


#endif // QTANGOIZERPRIVATE_H
