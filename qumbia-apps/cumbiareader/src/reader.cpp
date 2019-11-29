#include "reader.h"
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontext.h>
#include <cudata.h>

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include "tgdbprophelper.h"
#endif

Reader::Reader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QObject(parent)
{
    m_context = new CuContext(cumbia_pool, fpool);
    m_save_property = false;
    m_property_only = false;
}

Reader::~Reader()
{
    delete m_context;
}

void Reader::propertyOnly() {
    m_property_only = true;
}

void Reader::saveProperty()
{
    m_save_property = true;
}

void Reader::setTgPropertyList(const QStringList &props)
{
#ifdef QUMBIA_TANGO_CONTROLS_VERSION // otherwise always false
    m_tg_property_list = props;
#endif
}

void Reader::setContextOptions(const CuData &options) {
    m_context->setOptions(options);
}

void Reader::onUpdate(const CuData &da)
{
    CuData data(da);
    const CuVariant&  v = data["value"];
    double ts = data["timestamp_us"].toDouble();
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
    if(m_save_property) {
        // copy relevant property values into data
        foreach(QString p, QStringList() << "label" << "min" << "max" << "display_unit")
            data[qstoc(p)] = m_prop[qstoc(p)];
    }

    if(!da["err"].toBool() && !m_property_only) {
        if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Double)
            emit newDouble(src, ts, v.toDouble(), data);
        else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short)
            emit newShort(src, ts, v.toShortInt(), data);
        else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::LongInt)
            emit newLong(src, ts, v.toLongInt(), data);
        else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Double)
            emit newDoubleVector(src, ts, QVector<double>::fromStdVector(v.toDoubleVector()), data);
        else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Short)
            emit newShortVector(src, ts, QVector<short>::fromStdVector(v.toShortVector()), data);
        else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::LongInt) {
            emit newLongVector(src, ts, QVector<long>::fromStdVector(v.toLongIntVector()), data);
        }
        else if(!v.isNull())
            perr("Reader.onUpdate: unsupported data type %s and format %s in %s",
                 v.dataTypeStr(v.getType()).c_str(), v.dataFormatStr(v.getFormat()).c_str(),
                 data.toString().c_str());
    }
}

QString Reader::source() const
{
    if(CuControlsReaderA* r = m_context->getReader())
        return r->source();
    return "";
}

void Reader::stop()
{
    if(CuControlsReaderA* r = m_context->getReader())
        r->unsetSource();
}

void Reader::setPeriod(int ms)
{
    m_context->setOptions(CuData("period", ms));
}

void Reader::setSource(const QString &s)
{
    CuControlsReaderA * r = m_context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void Reader::getTgProps()
{
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    if(m_tg_property_list.size() > 0) {
        TgDbPropHelper *tdbh = new TgDbPropHelper(this);
        tdbh->get(m_context->cumbiaPool(), m_tg_property_list);
    }
#endif
}
