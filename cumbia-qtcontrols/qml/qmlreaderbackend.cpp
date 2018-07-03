#include "qmlreaderbackend.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cumbiapool_o.h>
#include <cudata.h>

#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"

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
    QString state_color, quality_color;
    QString label;
    CuContext *context;
    QmlReaderBackend::DataFormat data_fmt;
    QmlReaderBackend::DataType data_t;
};

QmlReaderBackend::QmlReaderBackend(QObject *parent) : QObject(parent)
{
    m_init();
    d->context = NULL;
}

void QmlReaderBackend::init(CumbiaPool_O *poo_o)
{
    d->context = new CuContext(poo_o->getPool(), poo_o->getFactory());
    if(!d->source.isEmpty())
        setSource(d->source);
}

void QmlReaderBackend::setConversionMode(QmlReaderBackend::DataType dt, QmlReaderBackend::DataFormat fmt)
{
    d->data_fmt = fmt;
    d->data_t = dt;
}

QString QmlReaderBackend::getQualityColor(double val)
{
    QString color = "#000000";
    if(d->max_alarm != d->min_alarm.toDouble() && (val >= d->max_alarm.toDouble() || val <= d->min_alarm.toDouble()))
        color = "red";
    else if(d->max_warning != d->min_warning.toDouble() && (val >= d->max_warning.toDouble() || val <= d->min_warning.toDouble()) )
        color = "orange";
    printf("QmlReaderBackend.getQualityColor: returning %s for %f\n", qstoc(color), val);
    return color;
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
    d->data_t = String; // unused
    d->data_fmt = Scalar; // unused
    d->state_color = "";
    d->quality_color = "white";

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

        QString label;
        if(da.containsKey("label"))
            label = QString::fromStdString(da["label"].toString());
        if(label != d->label) {
            d->label = label;
            emit labelChanged(label);
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
    return d->max_alarm;
}

QString QmlReaderBackend::label() const
{
    return d->label;
}

QString QmlReaderBackend::stateColor() const
{
    return d->state_color;
}

QString QmlReaderBackend::qualityColor() const
{
    return d->quality_color;
}

void QmlReaderBackend::onUpdate(const CuData &da)
{

    printf("\e[1;32mdata %s\e[0m\n", da.toString().c_str());

    d->read_ok = !da["err"].toBool();


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
    QString qs, sts;
    if(d->read_ok && da["quality_color"].isValid())
        qs = QString::fromStdString(da["quality_color"].toString());
    if(d->read_ok && da["state_color"].isValid())
        sts = QString::fromStdString(da["state_color"].toString());

    if(!d->read_ok) {
        m_set_value(CuVariant("####"));
        qs = "red";
        sts = "gray";
    }
    if(!qs.isEmpty() && qs != d->quality_color) {
        d->quality_color = qs;
        emit qualityColorChanged(d->quality_color);
    }
    if(!sts.isEmpty() && sts != d->state_color) {
        d->state_color = sts;
        emit stateColorChanged(d->state_color);
    }

    emit newData(da);
}

void QmlReaderBackend::setSource(const QString &s)
{
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
    QVariant v;
    if(val.isInteger()) {
        int i;
        val.to<int>(i);
        v = static_cast<double>(i);
    }
    else if(val.isFloatingPoint()) {
        double dou;
        val.to<double>(dou);
        v = dou;
    }
    else {
        v = QString::fromStdString(val.toString());
    }
    if(v != d->value) {
        d->value = v;
        emit valueChanged(v);
    }
}
