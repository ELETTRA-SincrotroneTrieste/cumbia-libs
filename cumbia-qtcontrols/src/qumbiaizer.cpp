#include "qumbiaizer.h"
#include "quvaluefilter.h"
#include <cumacros.h>
#include "qumbiaizerprivate.h"
#include <cudata.h>
#include <QRegularExpression>
#include <QWidget> /* to set tooltips on attached object, if it inherits QWidget */

Qumbiaizer::Qumbiaizer(QObject *parent) : QObject(parent)
{
    quizer_ptr = new QumbiaizerPrivate();
    quizer_ptr->type = Invalid;
    quizer_ptr->slot = QString();
    quizer_ptr->data = NULL;
}

Qumbiaizer::~Qumbiaizer()
{
    delete quizer_ptr;
}

bool Qumbiaizer::error() const
{
    return quizer_ptr->error;
}

QString Qumbiaizer::message() const
{
    return quizer_ptr->message;
}

/** \brief associate an auto configuration type to a slot to set the auto configuration value
 *
 * @param act an AutoConfType value
 * @param a slot that will be invoked at the configuration stage
 */
void Qumbiaizer::setAutoConfSlot(AutoConfType act, const char* slot)
{
    quizer_ptr->autoConfSlotsHash.insert(act, QString(slot));
}

/** \brief associate a QObject and one or two methods to be invoked by the class
  *        on new data arrival (QTWatcher) or on target execution (QTWriter).
  *
  * This method associates a QObject on which to invoke a slot when new data arrives
  * (for readers) or a QObject from which to wait for signals to execute a target
  * (for writers).
  *
  * @param object the QObject pointer
  * @param method the SLOT (readers) or SIGNAL (writers) methods to associate
  * @param setPointSlot if not NULL, this is the slot to be invoked in auto configuration
  *        to initialize the value read first.
  * @param connType the Qt::ConnectionType for signals/slots
  *
  * The type of input parameter is detected on the basis of the method specified.
  * For instance setValue(int) will detect an Int data type.
  * If you specify a setPointSlot, remember that its data type must be the same as
  * the data type of the method.
  *
  */
void Qumbiaizer::attach(QObject *refreshee,  const char *slot,
                        const char *setPointSlot,
                        Qt::ConnectionType connType)
{

    QString method(slot);
    QString setPtSlot;
    char in_type[TYPELEN], in_setPt_type[TYPELEN];
    bool typeOfMethodOk, typeOfSetPointSlotOk = true;
    quizer_ptr->extractCode(method); /* removes slot code from the signature */

    if(setPointSlot && strlen(setPointSlot) > 0)
    {
        setPtSlot = QString(setPointSlot);
        /* remove signal slot code from the signature */
        extractCode(setPtSlot);
    }

    typeOfMethodOk = inTypeOfMethod(method, refreshee, in_type);
    if(setPointSlot != NULL)
        typeOfSetPointSlotOk = inTypeOfMethod(setPtSlot, refreshee, in_setPt_type);

    if(typeOfMethodOk && typeOfSetPointSlotOk && setPointSlot != NULL && strcmp(in_setPt_type, in_type) != 0)
    {
        perr("QTangoizer::attach: input types for method and set point method for object \"%s\"",
             qstoc(refreshee->objectName()));
        perr("                    method in type \"%s\" set point method type \"%s\"", in_type, in_setPt_type);
    }
    else if(typeOfMethodOk && typeOfSetPointSlotOk)
    {
        if(strlen(in_type) == 0)
            quizer_ptr->type = Void;
        else if(strcmp(in_type, "bool") == 0)
            quizer_ptr->type = Bool;
        else if(strcmp(in_type, "int") == 0)
            quizer_ptr->type = Int;
        else if(strcmp(in_type, "double") == 0)
            quizer_ptr->type = Double;
        /* must be tested before the String if below */
        else  if(strcmp(in_type, "QStringList") == 0)
            quizer_ptr->type = StringVector;
        else if(strstr(in_type, "String") != NULL)
            quizer_ptr->type = String;
        else if(strcmp(in_type, "short") == 0)
            quizer_ptr->type = Short;
        else if(strcmp(in_type, "unsigned int") == 0)
            quizer_ptr->type = UInt;
        else  if(strcmp(in_type, "QVector<double>") == 0)
            quizer_ptr->type = DoubleVector;
        else  if(strcmp(in_type, "QVector<int>") == 0)
            quizer_ptr->type = IntVector;
        else  if(strcmp(in_type, "QVector<bool>") == 0)
            quizer_ptr->type = BoolVector;
        else if(strcmp(in_type, "CuData") == 0)
            quizer_ptr->type = Cu_Data;
        else
            perr("QTangoizer::attach() invalid or unrecognized type \"%s\"", in_type);

        quizer_ptr->data = refreshee;
        quizer_ptr->slot = method;
        /* save in method name only the name, without the parameter list */
        quizer_ptr->methodName = method.remove(QRegularExpression("\\(.*\\)"));
        quizer_ptr->connType = connType;
        if(!setPtSlot.isEmpty())
        {
            quizer_ptr->setPointSlot = setPtSlot;
            quizer_ptr->setPointMethodName = setPtSlot.remove(QRegularExpression("\\(.*\\)"));
        }
    }
    else
        perr("QTangoizer::attach: unsupported input type of method. Object \"%s\", slot \"%s\"",
             qstoc(refreshee->objectName()), qstoc(method));
}

bool Qumbiaizer::singleShot() const {
    return quizer_ptr->singleShot;
}

bool Qumbiaizer::autoDestroy() const {
    return quizer_ptr->autoDestroy;
}

void Qumbiaizer::setAutoDestroy(bool autoDestroy) {
    quizer_ptr->singleShot = true;
    quizer_ptr->autoDestroy = autoDestroy;
}

void Qumbiaizer::setSingleShot(bool singleShot) {
    quizer_ptr->singleShot = singleShot;
}

void Qumbiaizer::attach(short *s)
{

    quizer_ptr->data = s;
    quizer_ptr->type = Short;
}

void Qumbiaizer::attach(double *dou)
{

    quizer_ptr->data = dou;
    quizer_ptr->type = Double;
}

void Qumbiaizer::attach(int *i)
{

    quizer_ptr->data = i;
    quizer_ptr->type = Int;
}

void Qumbiaizer::attach(unsigned int *u)
{

    quizer_ptr->data = u;
    quizer_ptr->type = UInt;
}

void Qumbiaizer::attach(QString *qs)
{

    quizer_ptr->data = qs;
    quizer_ptr->type = String;
}

void Qumbiaizer::attach(bool *b)
{

    quizer_ptr->data = b;
    quizer_ptr->type = Bool;
}

void Qumbiaizer::attach( QStringList *sl)
{

    quizer_ptr->data = sl;
    quizer_ptr->type = StringVector;
}

void Qumbiaizer::attach( QVector<double> * v)
{

    quizer_ptr->data = v;
    quizer_ptr->type = DoubleVector;
}

void Qumbiaizer::attach( QVector<int> *vi)
{

    quizer_ptr->data = vi;
    quizer_ptr->type = IntVector;
}

void Qumbiaizer::attach( QVector<bool> *vb)
{

    quizer_ptr->data = vb;
    quizer_ptr->type = BoolVector;
}

void Qumbiaizer::attach(CuData *da) {
    quizer_ptr->data = da;
    quizer_ptr->type = Cu_Data;
}

void *Qumbiaizer::data() {
    return quizer_ptr->data;
}

QString Qumbiaizer::slot() const {
    return quizer_ptr->slot;
}

QString Qumbiaizer::methodName() const {
    return quizer_ptr->methodName;
}

QString Qumbiaizer::setPointSlot() const {
    return quizer_ptr->setPointSlot;
}

QString Qumbiaizer::setPointMethodName() const {
    return quizer_ptr->setPointMethodName;
}

Qumbiaizer::Type Qumbiaizer::type() const {
    return (Qumbiaizer::Type) quizer_ptr->type;
}

Qt::ConnectionType Qumbiaizer::connectionType() const {
    return quizer_ptr->connType;
}

void Qumbiaizer::removeRefreshFilter(QuValueFilter *filter) {
    Q_UNUSED(filter);
    quizer_ptr->refreshFilter = NULL;
}

void Qumbiaizer::installRefreshFilter(QuValueFilter *filter)
{

    quizer_ptr->refreshFilter = filter;
}

QuValueFilter *Qumbiaizer::refreshFilter() const {
    return quizer_ptr->refreshFilter;
}

void Qumbiaizer::setToolTipsDisabled(bool disable) {
    quizer_ptr->toolTipsDisabled = disable;
}

bool Qumbiaizer::toolTipsDisabled() const {
    return quizer_ptr->toolTipsDisabled;
}

const char *Qumbiaizer::type_str(Qumbiaizer::Type t) const
{
    switch(t) {
    case Invalid:
        return "Invalid";
    case Void:
        return "Void";
    case Int:
        return "Int";
    case Double:
        return "Double";
    case UInt:
        return "UInt";
    case Short:
        return "Short";
    case String:
        return "String";
    case Bool:
        return "Bool";
    case BoolVector:
        return "BoolVector";
    case DoubleVector:
        return "DoubleVector";
    case StringVector:
        return "StringVector";
    case IntVector:
        return "IntVector";
    case Cu_Data:
        return "CuData";
    default:
        return "Undefined";
    }
}

/** \brief Configures the attached object, if any.
 *
 * If setAutoconfSlot has been called for some  AutoConfType type, then the matching slots are invoked.
 * If a custom slot has not been specified for the minimum and maximum properties, the method
 * sets the <em>minimum</em> and <em>maximum</em> properties on the object, if they are defined.
 * If attach was called specifying an additional slot to set the <em>set point</em> value, then it is
 * invoked. This can be applied only to read/write quantities, when CuData contains the "w_value" value.
 * Tango and Epics support read/write quantities.
 *
 * The configured signal is emitted at the end. It can be used by the client to access other configuration
 * parameters in data.
 * The connectionOk signal is also emitted for convenience.
 *
 * @see setAutoconfSlot
 */
void Qumbiaizer::configure(const CuData &data)
{
    QObject *object = NULL;

    if(!quizer_ptr->slot.isEmpty())
        object = static_cast<QObject *>(quizer_ptr->data); /* QObject and slot were provided */

    if(object)
        quizer_ptr->configure(data, object);

    if(object && !quizer_ptr->setPointMethodName.isEmpty())
    {
        /* invoke updateValue with a false parameter to extract the set point from the
         * current value. newData signal is not emitted when the second param is false
         */
        updateValue(data, false, qstoc(quizer_ptr->setPointMethodName), QuValueFilter::AutoConfiguration);
    }

    emit configured(data);
    emit connectionOk(!data[CuDType::Err].toBool());  // data["err"]
}

bool Qumbiaizer::inTypeOfMethod(const QString &method, QObject *obj, char* in_type) {
    return quizer_ptr->inTypeOfMethod(method, obj, in_type);
}

int Qumbiaizer::extractCode(QString& method)
{

    return quizer_ptr->extractCode(method);
}

/*! \brief updates the listener with the new value
 *
 * @param v the new data
 * @param read true the update refers to a read value (default: true and newData signals are emitted)
 * @param read false the update refers to a *set point* value. In this case, no newData signal is emitted
 * @param customMethod if not null, the customMethod is invoked rather then the methodName
 * @param updateState either QuValueFilter::AutoConfiguration or QuValueFilter::Update (default QuValueFilter::Update)
 *
 * \par newData signals
 * A *type specific* newData signal is emitted if the data v contains a valid *value* key and no error has occurred.
 *
 * \par slot invocation
 * If an object and a *slot* were provided through the proper *attach* method, the *slot* is invoked on the object.
 */
void Qumbiaizer::updateValue(const CuData &v, bool read, const char* customMethod, QuValueFilter::State updateState)
{
    quizer_ptr->message.clear();
    const QString & methName = customMethod != NULL ? customMethod : methodName();
    bool ok = !v[CuDType::Err].toBool();

    if(ok && (v.containsKey(CuDType::Value) || v.containsKey(CuDType::WriteValue)))  {
        QObject *object = NULL;
        if(!quizer_ptr->slot.isEmpty())
            object = static_cast<QObject *>(quizer_ptr->data); /* QObject and slot were provided */

        const CuVariant &val = read ? v[CuDType::Value] : v[CuDType::WriteValue];
        if(type() == Qumbiaizer::Cu_Data) { // since 1.5.3
            CuData out(v); // inexpensive move operation
            if(quizer_ptr->refreshFilter) {
                quizer_ptr->refreshFilter->filter(v, out, read, updateState);
            }
            if(object) {
                ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(CuData, out));
                if(!ok)
                    quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                           + "\". ]";
            }
            else
                *static_cast<CuData *>(quizer_ptr->data) = v;
            if(read)
                emit newData(out);
        }
        else if(type() == Qumbiaizer::Int && val.getFormat() == CuVariant::Scalar)
        {
            int intVal;
            val.to<int>(intVal);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, intVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(int, intVal));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<int *>(data()) = intVal;
                /* emit even if invokeMethod fails */
                if(read)
                    emit newData(intVal);
            }
        }
        else if(type() == Qumbiaizer::Short && val.getFormat() == CuVariant::Scalar)
        {
            short shval;
            val.to<short>(shval);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, shval, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(short, shval));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<int *>(data()) = shval;
                /* emit even if invokeMethod fails */
                if(read)
                    emit newData(shval);
            }
            else
                quizer_ptr->message += " [cannot convert to short int] ";
        }

        else if(type() == Qumbiaizer::UInt && val.getFormat() == CuVariant::Scalar)
        {
            unsigned int uintVal;
            val.to<unsigned int>(uintVal);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, uintVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(unsigned int, uintVal));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<unsigned int *>(data()) = uintVal;
                if(read)
                    emit newData(uintVal);
            }
            else
                quizer_ptr->message += " [cannot convert to unsigned int] ";
        }

        else if(type() ==  Qumbiaizer::Double && val.getFormat() == CuVariant::Scalar)
        {
            double doubleVal;
            val.to<double>(doubleVal);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, doubleVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(double, doubleVal));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<double *>(data()) = doubleVal;
                if(read)
                    emit newData(doubleVal);
            }
            else
                quizer_ptr->message += " [cannot convert to double] ";
        }
        else if(type() == Qumbiaizer::String && (val.getFormat() == CuVariant::Scalar || val.getFormat() == CuVariant::Vector ) )
        {
            QString str = QString::fromStdString(val.toString(&ok));
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, str, read, updateState);

                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QString, str));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<QString *>(data()) = str;
                if(read)
                    emit newData(str);
            }
            else
                quizer_ptr->message += " [cannot convert to qt string] ";
        }

        else if(type() == Qumbiaizer::Bool && val.getFormat() == CuVariant::Scalar)
        {
            bool abool;
            val.to<bool>(abool);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, abool, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(bool, abool));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<bool *>(data()) = abool;
                if(read)
                    emit newData(abool);
            }
            else
                quizer_ptr->message += " [cannot convert to boolean] ";
        }

        else if(type() == Qumbiaizer::BoolVector && val.getFormat() == CuVariant::Vector)
        {
            std::vector<bool> stdbv;
            val.toVector<bool>(stdbv);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
            QVector<bool> boolvect(stdbv.begin(), stdbv.end());
#else
            QVector<bool> boolvect = QVector<bool>::fromStdVector(stdbv);
#endif
            if(quizer_ptr->refreshFilter)
                quizer_ptr->refreshFilter->filter(v, boolvect, read, updateState);
            if(object)
            {
                ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<bool>, boolvect));
                if(!ok)
                    quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                           + "\". ]";
            }
            else
                *static_cast<QVector<bool> *>(data()) = boolvect;
            if(read)
                emit newData(boolvect);
        }

        else if(type() == Qumbiaizer::DoubleVector && val.getFormat() == CuVariant::Vector)
        {
            std::vector<double> stddv;
            val.toVector<double>(stddv);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
            QVector<double> dblvect(stddv.begin(), stddv.end());
#else
            QVector<double> dblvect = QVector<double>::fromStdVector(stddv);
#endif
            if(quizer_ptr->refreshFilter)
                quizer_ptr->refreshFilter->filter(v, dblvect, read, updateState);
            if(object)
            {
                ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<double>, dblvect));
                if(!ok)
                    quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                           + "\". ]";
            }
            else
                *static_cast<QVector<double> *>(data()) = dblvect;
            if(read)
                emit newData(dblvect);
        }

        else if(type() == Qumbiaizer::IntVector && val.getFormat() == CuVariant::Vector)
        {
            std::vector<int> stdiv;
            val.toVector<int>(stdiv);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
            QVector<int> intvect(stdiv.begin(), stdiv.end());
#else
            QVector<int> intvect = QVector<int>::fromStdVector(stdiv);
#endif

            if(quizer_ptr->refreshFilter)
                quizer_ptr->refreshFilter->filter(v, intvect, read, updateState);
            if(object)
            {
                ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<int>, intvect));
                if(!ok)
                    quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                           + "\". ]";
            }
            else
                *static_cast<QVector<int> *>(data()) = intvect;
            if(read)
                emit newData(intvect);
        }
        else if(type() == Qumbiaizer::StringVector && val.getType() == CuVariant::String && val.getFormat() == CuVariant::Vector)
        {
            QStringList strvect;
            foreach(std::string s, val.toStringVector(&ok))
                strvect << QString::fromStdString(s);
            if(ok)
            {
                if(quizer_ptr->refreshFilter)
                    quizer_ptr->refreshFilter->filter(v, strvect, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QStringList, strvect));
                    if(!ok)
                        quizer_ptr->message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                               + "\". ]";
                }
                else
                    *static_cast<QStringList *>(data()) = strvect;
                if(read)
                    emit newData(strvect);
            }
            else
                quizer_ptr->message += " [cannot convert to string vector] ";
        }
        else
        {
            ok = false;
            quizer_ptr->message = QString(" [ invalid Type %1 [%2] (CuVariant type  %3 [%4]) specified ]").arg(type()).arg(type_str(type())).
                                  arg(val.getType()).arg(val.dataTypeStr(val.getType()).c_str());
        }

        if(object && !ok)
            perr("Qumbiaizer.updateValue: failed to invoke method \"%s\" on \"%s\": \"%s\"", qstoc(methName),
                 qstoc(object->objectName()), qstoc(quizer_ptr->message));
    }
}


