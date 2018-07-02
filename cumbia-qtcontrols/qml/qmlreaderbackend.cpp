#include "qmlreaderbackend.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>

#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"
#include "cumbiapoolfactory.h"

#include <QtDebug>

/** @private */
class QmlReaderBackendPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QVariant value;
    QVariant min, max;
    QVariant min_warning, max_warning;
    QVariant min_alarm, max_alarm;
    QString source;
    CuContext *context;
};

QmlReaderBackend::QmlReaderBackend(QObject *parent) : QObject(parent)
{
    m_init();
    d->context = NULL;
    printf("QmlReaderBackend::QmlReaderBackend(QObject *parent) this %p \n", this);
}

void QmlReaderBackend::init(CumbiaPoolFactory *poof)
{
    printf("\e[1;32mQmlReaderBackend::init poof %p\e[0m\n", poof);
    d->context = new CuContext(poof->getPool(), poof->getFactory());
    if(!d->source.isEmpty())
        setSource(d->source);
}

QmlReaderBackend::~QmlReaderBackend()
{
    pdelete("~QmlReaderBackend %p", this);
    delete d->context;
    delete d;
}

void QmlReaderBackend::m_init()
{
    d = new QmlReaderBackendPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
    d->value = 0.0;
    d->min = 0;
    d->max = 100;
    d->min_alarm = 0.0;
    d->max_alarm = 0.0;
    d->min_warning = 0.0;
    d->max_warning = 0.0;
}

void QmlReaderBackend::m_configure(const CuData &da)
{
    QVariant v;
    try {
        if(da.containsKey("min"))
            v = QVariant(std::stod(da["min"].toString()));
        if(d->min != v) {
            d->min = v;
            emit minChanged(v);
        }
        if(da.containsKey("max"))
            v = QVariant(std::stod(da["max"].toString()));
        if(d->max != v) {
            d->max = v;
            emit maxChanged(v);
        }


        if(da.containsKey("min_warning"))
            v = QVariant(std::stod(da["min_warning"].toString()));
        if(d->min_warning != v) {
            d->min_warning = v;
            emit min_warningChanged(v);
        }
        if(da.containsKey("max_warning"))
            v = QVariant(std::stod(da["max_warning"].toString()));
        if(d->max_warning != v) {
            d->max_warning = v;
            emit max_warningChanged(v);
        }

        if(da.containsKey("min_alarm"))
            v = QVariant(std::stod(da["min_alarm"].toString()));
        if(d->min_alarm != v) {
            d->min_alarm = v;
            emit min_alarmChanged(v);
        }
        if(da.containsKey("max_alarm"))
            v = QVariant(std::stod(da["max_alarm"].toString()));
        if(d->max_alarm != v) {
            d->max_alarm = v;
            emit max_alarmChanged(v);
        }
    }
    catch(const std::invalid_argument& ia) {
        perr("QmlReaderBackend.m_configure: number coversion error: %s", ia.what());
    }
}

QString QmlReaderBackend::source() const
{
    CuControlsReaderA* r;
    if(d->context && ( r = d->context->getReader()) != NULL)
        return r->source();
    return "";
}

CuContext *QmlReaderBackend::getContext() const
{
    return d->context;
}

QVariant QmlReaderBackend::value() const
{
    printf("\e[1;32mreturning value %f\e[0m\n", d->value.toDouble());
    return d->value;
}

QVariant QmlReaderBackend::min() const
{
    return d->min;
}

QVariant QmlReaderBackend::max() const
{
    return d->max;
}

QVariant QmlReaderBackend::min_warning() const
{
    return d->min_warning;
}

QVariant QmlReaderBackend::max_warning() const
{
    return d->max_warning;
}

QVariant QmlReaderBackend::min_alarm() const
{
    return d->min_alarm;
}

QVariant QmlReaderBackend::max_alarm() const
{
    printf("\e[1;32mreturning max_alarm %f\e[0m\n", d->max_alarm.toDouble());
    return d->max_alarm;
}

void QmlReaderBackend::onUpdate(const CuData &da)
{
    printf("\e[1;33m QuCircularGauge::onUpdate da %s\e[0m\n", da.toString().c_str());
    d->read_ok = !da["err"].toBool();
    if(!d->read_ok) {

    }

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    if(d->read_ok && da["value"].isValid()) {
        m_set_value(da["value"]);
    }
    emit newData(da);
}

void QmlReaderBackend::setSource(const QString &s)
{
    printf("\e[1;32msetSource %s this %p\e[0m\n", qstoc(s), this);
    if(d->context) {
        CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
        if(r && s != r->source()) {
            r->setSource(s);
        }
    }
    if(d->source != s) {
        d->source = s;
        emit sourceChanged(s);
    }
}

void QmlReaderBackend::unsetSource()
{
    d->context->disposeReader();
}

void QmlReaderBackend::m_set_value(const CuVariant &val)
{
    double v;
    if(val.isInteger()) {
        int i;
        val.to<int>(i);
        v = static_cast<double>(i);
    }
    else if(val.isFloatingPoint()) {
        val.to<double>(v);
    }
    else {
        perr("wrong data type %s" , val.dataTypeStr(val.getType()).c_str());
    }
    if(v != d->value) {
        d->value = v;
        emit valueChanged(v);
    }
}
