#include "qumbiaizerprivate.h"
#include <QMetaMethod>
#include <cumacros.h>
#include <cudata.h>
#include <cudatatypes_ex.h>

#define TYPELEN 64

QumbiaizerPrivate::QumbiaizerPrivate()
{
    refreshFilter = NULL;
    singleShot = false;
    autoDestroy = false;
    m_executeOnConnection = false;
    toolTipsDisabled = false;
    error = false;
}

QumbiaizerPrivate::~QumbiaizerPrivate()
{

}

void QumbiaizerPrivate::setExecuteOnConnection(bool execute, const CuVariant &argument)
{
    m_executeOnConnection = execute;
    m_executeArgument = argument;
}

bool QumbiaizerPrivate::inTypeOfMethod(const QString &method, QObject *obj, char* in_type)
{
    memset(in_type, 0, TYPELEN);
    int idx = obj->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(qstoc(method)));
    if(idx > -1)
    {
        QMetaMethod metaMethod = obj->metaObject()->method(idx);
        QList<QByteArray> parTypes = metaMethod.parameterTypes();
        if(parTypes.size() == 1)
        {
            strncpy(in_type, parTypes.first().constData(), TYPELEN - 1);
            return true;
        }
        else if(parTypes.size() == 0) /* void */
            return true;
    }
    else
        printf("\e[1;31mmethod \"%s\" on obj \"%s\" not found!\e[0m\n", qstoc(method), qstoc(obj->objectName()));
    return false;
}

bool QumbiaizerPrivate::configure(const CuData &da, QObject *object)
{
    if(da[CuDType::Type].toString() != "property")
        return false;

    Q_Q(Qumbiaizer);
    bool ret = true;
    char in_type[TYPELEN];
    double value;
    void *dataptr;
    QString u, methodName;
    /* minimum */
    bool has_min = da.containsKey(CuXDType::Min);
    bool has_max = da.containsKey(CuXDType::Max);
    double v;
    double min, max;
    if(has_min && autoConfSlotsHash.contains(q->Min))
    {
        da[CuXDType::Min].to<double>(min); // cuvariant_t.h
        methodName = autoConfSlotsHash.value(q->Min);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, min);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object,
                    qstoc(methodName.remove(QRegExp("\\(.*\\)"))),
                    connType, QGenericArgument(in_type, dataptr));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);

            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    else if(has_min && object->metaObject()->indexOfProperty("minimum") > -1)
    {
        da[CuXDType::Min].to<double>(min);
        object->setProperty("minimum", min);
    }

 ///   printf("cp->minIsSet %d autoconf contains min %d \n", cp->minIsSet(), autoConfSlotsHash.contains(q->Min));
    /* maximum */
    if(has_max && autoConfSlotsHash.contains(q->Max))
    {
        da[CuXDType::Max].to<double>(max); // cuvariant_t.h converts to long double
        methodName = autoConfSlotsHash.value(q->Max);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, max);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object,
                   qstoc(methodName.remove(QRegExp("\\(.*\\)"))), connType,
                   QGenericArgument(in_type, dataptr));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);

            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    else if(has_max && object->metaObject()->indexOfProperty("maximum") > -1)
    {
        da[CuXDType::Max].to<double>(min);
        object->setProperty("maximum", min);
    }

    /* min warning */
    if(da.containsKey(CuXDType::LowerWarningLimit) && autoConfSlotsHash.contains(q->MinWarn))
    {
        da[CuXDType::LowerWarningLimit].to<double>(v);
        methodName = autoConfSlotsHash.value(q->MinWarn);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, v);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MinWarn)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* max warning */
    if(da.containsKey(CuXDType::UpperWarningLimit)  && autoConfSlotsHash.contains(q->MaxWarn))
    {
        da[CuXDType::UpperWarningLimit].to<double>(v);
        methodName = autoConfSlotsHash.value(q->MaxWarn);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, v);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MaxWarn)), connType,
                         QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* min alarm */
    if(da.containsKey(CuXDType::LowerAlarmLimit) && autoConfSlotsHash.contains(q->MinErr))
    {
        da[CuXDType::LowerAlarmLimit].to<double>(v);
        methodName = autoConfSlotsHash.value(q->MinErr);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, v);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MinErr)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* max alarm */
    if(da.containsKey(CuXDType::UpperAlarmLimit)  && autoConfSlotsHash.contains(q->MaxErr))
    {
        da[CuXDType::UpperAlarmLimit].to<double>(v);
        methodName = autoConfSlotsHash.value(q->MaxErr);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {

            dataptr = m_getDataPtr(in_type, v);
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MaxErr)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    if(da.containsKey(CuXDType::StandardUnit) && autoConfSlotsHash.contains(q->StdUnit))
    {
        methodName = autoConfSlotsHash.value(q->StdUnit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = QString::fromStdString(da[CuXDType::StandardUnit].toString());
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->StdUnit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(da.containsKey(CuXDType::Unit) && autoConfSlotsHash.contains(q->Unit))
    {
        methodName = autoConfSlotsHash.value(q->Unit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = QString::fromStdString(da[CuXDType::Unit].toString());
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->Unit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(da.containsKey(CuXDType::DisplayUnit)   && autoConfSlotsHash.contains(q->DisplayUnit))
    {
        methodName = autoConfSlotsHash.value(q->DisplayUnit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = QString::fromStdString(da[CuXDType::DisplayUnit].toString());
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->DisplayUnit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(da.containsKey(CuXDType::Label) &&autoConfSlotsHash.contains(q->Label))
    {
        methodName = autoConfSlotsHash.value(q->Label);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = QString::fromStdString(da[CuXDType::Label].toString());
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->Label)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }

    if(!ret)
        perr("QTWatcher::attributeAutoConfigured: one or more invokeMethod() failed");
    return ret;
}

void *QumbiaizerPrivate::m_getDataPtr(const char *type, double val)
{
    void *dataptr = NULL;
    if(strcmp(type, "int") == 0)
    {
        int *i = new int[1];
        *i = (int) val;
        dataptr = i;
    }
    else if(strcmp(type, "double") == 0)
    {
        double *d = new double[1];
        *d = val;
        dataptr = d;
    }
    else if(strcmp(type, "unsigned int") == 0)
    {
        unsigned int *ui = new unsigned int[1];
        *ui = (unsigned int) val;
        dataptr = ui;
    }
    else if(strcmp(type, "float") == 0)
    {
        float *f = new float[1];
        *f = val;
        dataptr = f;
    }
    else if(strcmp(type, "short") == 0)
    {
        short *s = new short[1];
        *s = val;
        dataptr = s;
    }
    else if(strcmp(type, "unsigned short") == 0)
    {
        unsigned short *us = new unsigned short[1];
        *us = (unsigned short) val;
        dataptr = us;
    }
    return dataptr;
}

void QumbiaizerPrivate::m_DeleteDataPtr(const char *type, void *dataptr)
{
    if(strcmp(type, "int") == 0)
        delete (int *) dataptr;
    else if(strcmp(type, "double") == 0)
        delete (double *) dataptr;
    else if(strcmp(type, "unsigned int") == 0)
         delete (unsigned int *) dataptr;
    else if(strcmp(type, "float") == 0)
        delete (float *) dataptr;
    else if(strcmp(type, "short") == 0)
        delete (short *) dataptr;
    else if(strcmp(type, "unsigned short") == 0)
        delete (unsigned short *) dataptr;
}

int QumbiaizerPrivate::extractCode(QString &method)
{
    if(method.size() > 1)
    {
      int code = QString(method.at(0)).toInt();
      if(code > QMETHOD_CODE && code <= QSIGNAL_CODE)
          method.remove(0, 1);
    }
    return -1;
}

void QumbiaizerPrivate::setError(bool err)
{
    error = err;
}

