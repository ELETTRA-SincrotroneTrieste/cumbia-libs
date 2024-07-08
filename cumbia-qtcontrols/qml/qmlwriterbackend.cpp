#include "qmlwriterbackend.h"
#include "cucontrolswriter_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cumbiapool_o.h>
#include <cudata.h>
#include <QStringList>
#include <vector>

#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <culinkstats.h>
#include <cucontextmenu.h>
#include <cucontext.h>
#include <culog.h>
#include <cuserviceprovider.h>
#include <cuservices.h>
#include <qulogimpl.h>

/** @private */
class QmlWriterBackendPrivate
{
public:
    bool auto_configure;
    bool ok;
    CuContext *context;
    QString target, unit, description;
    QString label;
    QVariant min, max, value;
};

QmlWriterBackend::QmlWriterBackend(QObject *parent) :
    QObject(parent), CuDataListener()
{
    m_init();
}

void QmlWriterBackend::m_init()
{
    d = new QmlWriterBackendPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->ok = true;
    d->min = 0.0;
    d->max = 1.0;
    d->value = 0.0;
}

void QmlWriterBackend::init(CumbiaPool_O *poo_o)
{
    d->context = new CuContext(poo_o->getPool(), poo_o->getFactory());
    if(!d->target.isEmpty())
        setTarget(d->target);
}

QmlWriterBackend::~QmlWriterBackend()
{
    pdelete("~QmlWriterBackend %p", this);
    if(d->context)
        delete d->context;
    delete d;
}

QString QmlWriterBackend::target() const {
    if(d->context) {
        CuControlsWriterA *w = d->context->getWriter();
        if(w != NULL)
            return w->target();
    }
    return "";
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *QmlWriterBackend::getContext() const
{
    return d->context;
}

QVariant QmlWriterBackend::min() const
{
    return d->min;
}

QVariant QmlWriterBackend::max() const
{
    return d->max;
}

QVariant QmlWriterBackend::value() const
{
    return d->value;
}

QString QmlWriterBackend::label() const
{
    return d->label;
}

QString QmlWriterBackend::unit() const
{
    return d->unit;
}

QString QmlWriterBackend::description() const
{
    return d->description;
}

bool QmlWriterBackend::ok() const
{
    return d->ok;
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QmlWriterBackend::setTarget(const QString &target)
{
    if(d->context) {
        CuControlsWriterA* w = d->context->replace_writer(target.toStdString(), this);
        if(w && target != w->target()) {
            w->setTarget(target);
        }
    }
    if(d->target != target) {
        d->target = target;
        emit targetChanged();
    }
}

void QmlWriterBackend::m_configure(const CuData& da)
{
    QString description, unit, label;
    QVariant v;

    printf("QmlWriterBackend.m_configure: da: %s\n", da.toString().c_str());
    // unit, label, description
    unit = QString::fromStdString(da["display_unit"].toString());
    label = QString::fromStdString(da[TTT::Label].toString());  // da["label"]
    description = QString::fromStdString(da[TTT::Description].toString());  // da["description"]
    if(unit != d->unit) {
        d->unit = unit;
        emit unitChanged();
    }
    if(label != d->label) {
        d->label = label;
        emit labelChanged();
    }
    if(description != d->description) {
        d->description = description;
        emit descriptionChanged();
    }

    // min, max
    try {
        if(da.containsKey(TTT::Min))  // da.containsKey("min")
            v = QVariant(strtod(da[TTT::Min].toString().c_str(), NULL));  // da["min"]
        if(d->min != v) {
            d->min = v;
            emit minChanged();
        }
        if(da.containsKey(TTT::Max))  // da.containsKey("max")
            v = QVariant(strtod(da[TTT::Max].toString().c_str(), NULL));  // da["max"]
        if(d->max != v) {
            d->max = v;
            emit maxChanged();
        }
    }

    catch(const std::invalid_argument& ia) {
        perr("QmlReaderBackend.m_configure: number coversion error: %s", ia.what());
    }
    // initialise the object with the "write" value (also called "set point"), if available:
    //
    if(d->ok && da.containsKey(TTT::WriteValue)) {  // da.containsKey("w_value")
        CuVariant var = da[TTT::WriteValue];  // da["w_value"]
        if(var.getFormat() == CuVariant::Scalar) {
            if(var.isFloatingPoint() || var.isInteger()) {
                double dval;
                var.to<double>(dval);
                if(dval != d->value) {
                    d->value = dval;
                    printf("emitting value changed value is %f\n", dval);
                    emit valueChanged();
                }
            }
            else if(var.getType() == CuVariant::String) {
                QVariant str = QString::fromStdString(var.toString());
                if(d->value != str) {
                    d->value = str;
                    emit valueChanged();
                }
            }
        }
    }
}

void QmlWriterBackend::onUpdate(const CuData &da)
{
    bool ok = !da[TTT::Err].toBool();  // da["err"]
    if(d->ok != ok) {
        d->ok = ok;
        emit okChanged();
    }

    if(!d->ok) {
        perr("QmlWriterBackend [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[TTT::Src].toString().c_str(), da[TTT::Message].toString().c_str(),  // da["src"], da["msg"]
                da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[TTT::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QmlWriterBackend [" + objectName() + "]").toStdString(), da[TTT::Message].toString(), CuLog::LevelError, CuLog::CategoryWrite);  // da["msg"]
        }
    }
    else if(d->auto_configure && da[TTT::Type].toString() == "property") {  // da["type"]
        //
        // --------------------------------------------------------------------------------------------
        // You may want to check data format and write type and issue a warning or avoid configuration
        // at all if they are not as expected
        // if(da["dfs"] == "scalar" && da["writable"].toInt() > 0)
        // --------------------------------------------------------------------------------------------
        m_configure(da);
    }
    emit newData(da);
}

/** \brief write an integer to the target
 *
 * @param i the value to be written on the target
 */
void QmlWriterBackend::write(QVariant ival) {
    printf("\e[1;33mQmlWriteBackend.write: to scalar only (%f, type %s, isvalid %d is null %d)!\e[0m\n",
           ival.toDouble(), ival.typeName(), ival.isValid(), ival.isNull());
    if(ival.isValid() && ival.type() == QVariant::Double)
        m_write(CuVariant(ival.toDouble()));
    else if(ival.isValid() && ival.type() == QVariant::UInt)
        m_write(CuVariant(ival.toUInt()));
    else if(ival.isValid() && ival.type() == QVariant::ULongLong)
        m_write(CuVariant(ival.toULongLong()));
    else if(ival.isValid() && ival.type() == QVariant::Int)
        m_write(CuVariant(ival.toInt()));
    else if(ival.isValid() && ival.type() == QVariant::LongLong) {
        unsigned long long int ulli = static_cast<unsigned long long>(ival.toULongLong());
        m_write(CuVariant(ulli));
    }
    else if(ival.isValid() && ival.type() == QVariant::String)
        m_write(CuVariant(ival.toString().toStdString()));
    else if(ival.isValid() && ival.type() == QVariant::StringList) {

    }
    else if(!ival.isValid() || ival.isNull()) {
        printf("m_write with empty CuVariant\n");
        m_write(CuVariant());
    }

}

// perform the write operation on the target
//
void QmlWriterBackend::m_write(const CuVariant& v)
{
    if(!d->context)
        return;
    CuControlsWriterA *w = d->context->getWriter();
    if(w) {
        w->setArgs(v);
        w->execute();
    }
}

