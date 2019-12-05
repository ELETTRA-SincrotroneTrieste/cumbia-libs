#include "qureader.h"
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontext.h>
#include <cudata.h>
#include <qustring.h>
#include <qustringlist.h>

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include "tgdbprophelper.h"
#endif

Qu_Reader::Qu_Reader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QObject(parent)
{
    m_context = new CuContext(cumbia_pool, fpool);
    m_save_property = false;
    m_property_only = false;
}

Qu_Reader::~Qu_Reader()
{
    delete m_context;
}

void Qu_Reader::propertyOnly() {
    m_property_only = true;
}

void Qu_Reader::saveProperty()
{
    m_save_property = true;
}

void Qu_Reader::setTgPropertyList(const QStringList &props)
{
#ifdef QUMBIA_TANGO_CONTROLS_VERSION // otherwise always false
    m_tg_property_list = props;
#endif
}

void Qu_Reader::setContextOptions(const CuData &options) {
    m_context->setOptions(options);
}

// delivers newData signals.
// manages property data and merges it with value data if necessary
// before notification (EPICS configuration arrives after first data)
//
void Qu_Reader::onUpdate(const CuData &da)
{
    CuData data(da);
    const CuVariant&  v = data["value"];
    double ts = -1.0;
    if(!da["timestemp_us"].isNull())
        ts = da["timestamp_us"].toDouble() * 1000;
    else if(!da["timestamp_ns"].isNull())
        ts = da["timestamp_ns"].toDouble();
    QString src = QString::fromStdString(data["src"].toString());
    if(data["err"].toBool())
        emit newError(src, ts, QString::fromStdString(data["msg"].toString()), data);
    else if(m_tg_property_list.size() > 0 && data.containsKey("list")) {
        // use propertyReady: receiver will use the "list" key to print values
        emit propertyReady(src, ts, data);
    }
    else if(data.has("type", "property"))  {
        if(m_save_property)
            m_prop = data;
        if(m_property_only) {
            emit propertyReady(src, ts, data);
        }
    }
    if(m_save_property && !m_prop.isEmpty()) {
        // copy relevant property values into data
        foreach(QString p, QStringList() << "label" << "min" << "max" << "display_unit")
            data[qstoc(p)] = m_prop[qstoc(p)];
    }

    if(!da["err"].toBool() && !m_property_only) {
        // if !m_save property we can notify.
        // otherwise wait for property, merge m_prop with data and notify
        // (epics properties are not guaranteed to be delivered first)
        if(!m_save_property || (m_save_property && !m_prop.isEmpty()) ) {
            if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Double)
                emit newDouble(src, ts, v.toDouble(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Float)
                emit newFloat(src, ts, v.toFloat(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Boolean)
                emit newBool(src, ts, v.toBool(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short)
                emit newShort(src, ts, v.toShortInt(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::LongUInt)
                emit newULong(src, ts, v.toULongInt(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::LongInt)
                emit newLong(src, ts, v.toLongInt(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short)
                emit newShort(src, ts, v.toShortInt(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::UShort)
                emit newUShort(src, ts, v.toUShortInt(), data);
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::String)
                emit newString(src, ts, QuString(v.toString()), data);
            else if(v.getFormat() == CuVariant::Scalar) {
                QString from_ty = QuString(v.dataTypeStr(v.getType()));
                emit toString(src, from_ty, ts, QuString(v.toString()), data);
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Double)
                emit newDoubleVector(src, ts, QVector<double>::fromStdVector(v.toDoubleVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Float)
                emit newFloatVector(src, ts, QVector<float>::fromStdVector(v.toFloatVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Boolean)
                emit newBoolVector(src, ts, QVector<bool>::fromStdVector(v.toBoolVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::LongUInt)
                emit newULongVector(src, ts, QVector<unsigned long>::fromStdVector(v.toULongIntVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Short)
                emit newShortVector(src, ts, QVector<short>::fromStdVector(v.toShortVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::UShort)
                emit newUShortVector(src, ts, QVector<unsigned short>::fromStdVector(v.toUShortVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::LongInt)
                emit newLongVector(src, ts, QVector<long>::fromStdVector(v.toLongIntVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Short)
                emit newShortVector(src, ts, QVector<short>::fromStdVector(v.toShortVector()), data);
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::String)
                emit newStringList(src, ts, QuStringList(v.toStringVector()), data);
            else if(v.getFormat() == CuVariant::Vector) {
                QString from_ty = QuString(v.dataTypeStr(v.getType()));
                emit toStringList(src, from_ty, ts, QuStringList(v.toStringVector()), data);
            }

            else if(!v.isNull()) {
                data["err"] = true;
                QString msg = QString("Reader.onUpdate: unsupported data type %1 and format %2 in %3")
                        .arg(v.dataTypeStr(v.getType()).c_str()).arg(v.dataFormatStr(v.getFormat()).c_str())
                        .arg(data.toString().c_str());
                        perr("%s", qstoc(msg));
                emit newError(src, ts, msg, data);
            }
        }
    }
}

QString Qu_Reader::source() const
{
    if(CuControlsReaderA* r = m_context->getReader())
        return r->source();
    return "";
}

void Qu_Reader::stop()
{
    if(CuControlsReaderA* r = m_context->getReader())
        r->unsetSource();
}

void Qu_Reader::setPeriod(int ms)
{
    m_context->setOptions(CuData("period", ms));
}

void Qu_Reader::setSource(const QString &s)
{
    CuControlsReaderA * r = m_context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void Qu_Reader::getTgProps()
{
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    if(m_tg_property_list.size() > 0) {
        TgDbPropHelper *tdbh = new TgDbPropHelper(this);
        tdbh->get(m_context->cumbiaPool(), m_tg_property_list);
    }
#endif
}
