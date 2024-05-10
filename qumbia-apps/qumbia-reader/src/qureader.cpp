#include "qureader.h"
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontext.h>
#include <cudata.h>
#include <qustring.h>
#include <qustringlist.h>
#include <QtDebug>

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

Qu_Reader::~Qu_Reader() {
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
void Qu_Reader::onUpdate(const CuData &da) {
    bool signaled = false;
    bool property_only = m_property_only || da.has(CuDType::Activity, "cutadb");  // has("activity", "cutadb")
    CuData data = da.clone();
    const CuVariant&  v = da[CuDType::Value];
    double ts = -1.0;
    if(!data[CuDType::Time_us].isNull()) {
        ts = data[CuDType::Time_us].toDouble();
    }
    else if(!data[CuDType::Time_ns].isNull())
        ts = data[CuDType::Time_ns].toDouble();

    bool hdb_data = data.has(CuDType::Activity, "hdb");
    if(data.B(CuDType::Err)) {
        emit newError(source(), ts, QString::fromStdString(da[CuDType::Message].toString()), data);  // da["msg"]
        signaled = true;
    }
    else if(hdb_data) {
        emit newHdbData(source(), data);
        signaled = true;
    }
    else if(m_tg_property_list.size() > 0 && data.containsKey("list")) {
        // use propertyReady: receiver will use the "list" key to print values
        emit propertyReady(source(), ts, data);
        signaled = true;
    }
    else if(da.has(CuDType::Type, "property"))  {  // has("type", "property")
        if(m_save_property)
            m_prop = data;
        if(property_only) {
            emit propertyReady(source(), ts, data);
            signaled = true;
        }
    }
    else if(m_save_property || property_only) {
        emit newError(source(), ts, "missing 'type'='property' in data", data);
        signaled = true;
    }

    if(!hdb_data && m_save_property && !m_prop.isEmpty()) {
        // copy relevant property values into data
        foreach(QString p, QStringList() << "label" << "min" << "max" << "display_unit")
            if(m_prop[qstoc(p)].isValid())
                data[qstoc(p)] = m_prop[qstoc(p)];
    }
    if(!da.B(CuDType::Err) && ts > 0 && !da.containsKey(CuDType::Value) && !da.containsKey(CuDType::WriteValue) && da.containsKey(CuDType::Src))  { // da.B("err"), da.containsKey("value"), da.containsKey("w_value"), da.containsKey("src")
        emit newUnchanged(source(), ts);
        signaled = true;
    }

    if(!hdb_data && !da.B(CuDType::Err) && !property_only) {
        QString from_ty = QuString(v.dataTypeStr(v.getType()));
        // if !m_save property we can notify.
        // otherwise wait for property, merge m_prop with data and notify
        // (epics properties are not guaranteed to be delivered first)
        if(!m_save_property || (m_save_property && !m_prop.isEmpty()) ) {
            if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Double) {
                emit newDouble(source(), ts, v.toDouble(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Float) {
                emit newFloat(source(), ts, v.toFloat(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Boolean) {
                emit newBool(source(), ts, v.toBool(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short) {
                emit newShort(source(), ts, v.toShortInt(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::LongUInt) {
                emit newULong(source(), ts, v.toULongInt(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::LongInt) {
                emit newLong(source(), ts, v.toLongInt(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short) {
                emit newShort(source(), ts, v.toShortInt(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::UShort) {
                emit newUShort(source(), ts, v.toUShortInt(), data);
            }
            else if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::String) {
                emit newString(source(), ts, QuString(v.toString()), data);
            }
            else if(v.getFormat() == CuVariant::Scalar) {
                QString from_ty = QuString(v.dataTypeStr(v.getType()));
                emit toString(source(), from_ty, ts, QuString(v.toString()), data);
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Double) {
                const std::vector<double> &vd = v.toDoubleVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newDoubleVector(source(), ts, QVector<double>(vd.begin(), vd.end()), data);
#else
                emit newDoubleVector(source(), ts, QVector<double>::fromStdVector(vd), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Float) {
                const std::vector<float> &vf = v.toFloatVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newFloatVector(source(), ts, QVector<float>(vf.begin(), vf.end()), data);
#else
                emit newFloatVector(source(), ts, QVector<float>::fromStdVector(vf), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Boolean) {
                const std::vector<bool> &vb = v.toBoolVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newBoolVector(source(), ts, QVector<bool>(vb.begin(), vb.end()), data);
#else
                emit newBoolVector(source(), ts, QVector<bool>::fromStdVector(vb), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::LongUInt) {
                const std::vector<unsigned long> &vlu = v.toULongIntVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newULongVector(source(), ts, QVector<unsigned long>(vlu.begin(), vlu.end()), data);
#else
                emit newULongVector(source(), ts, QVector<unsigned long>::fromStdVector(vlu), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Short) {
                const std::vector<short> &vsu = v.toShortVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newShortVector(source(), ts, QVector<short>(vsu.begin(), vsu.end()), data);
#else
                emit newShortVector(source(), ts, QVector<short>::fromStdVector(vsu), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::UShort) {
                const std::vector<unsigned short> &vsu = v.toUShortVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newUShortVector(source(), ts, QVector<unsigned short>(vsu.begin(), vsu.end()), data);
#else
                emit newUShortVector(source(), ts, QVector<unsigned short>::fromStdVector(vsu), data);
#endif
            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::LongInt) {
                const std::vector<long int> &vli = v.toLongIntVector();
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
                emit newLongVector(source(), ts, QVector<long int>(vli.begin(), vli.end()), data);
#else
                emit newLongVector(source(), ts, QVector<long int>::fromStdVector(vli), data);
#endif

            }
            else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::String) {
                emit newStringList(source(), ts, QuStringList(v.toStringVector()), data);
            }
            else if(v.getFormat() == CuVariant::Vector) {
                emit toStringList(source(), from_ty, ts, QuStringList(v.toStringVector()), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::Double) {
                emit newDoubleMatrix(source(),  ts, v.toMatrix<double>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::Float) {
                emit newFloatMatrix(source(),  ts, v.toMatrix<float>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::Boolean) {
                emit newBoolMatrix(source(),  ts, v.toMatrix<bool>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::UChar) {
                emit newUCharMatrix(source(),ts, v.toMatrix<unsigned char>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::Char) {
                emit newCharMatrix(source(),ts, v.toMatrix< char>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::Short) {
                emit newShortMatrix(source(), ts, v.toMatrix<short>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::UShort) {
                emit newUShortMatrix(source(), ts, v.toMatrix<unsigned short>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::LongInt) {
                emit newLongMatrix(source(),ts, v.toMatrix<long int>(), data);
            }
            else if(v.getFormat() == CuVariant::Matrix && v.getType() == CuVariant::String) {
                emit newStringMatrix(source(), ts, v.toMatrix<std::string>(), data);
            }
            else if(!v.isNull()) {
                data[CuDType::Err] = true;  // data["err"]
                QString msg = QString("Reader.onUpdate: unsupported data type %1 and format %2 in %3")
                                  .arg(v.dataTypeStr(v.getType()).c_str()).arg(v.dataFormatStr(v.getFormat()).c_str())
                                  .arg(data.toString().c_str());
                perr("%s", qstoc(msg));
                emit newError(source(), ts, msg, data);
            }
            else
                emit newError(source(), ts, "error in data", data);
            signaled = true;
        }
    }
    if(!signaled)
        emit newError(source(), ts, "error processing data", data);
}

QString Qu_Reader::source() const
{
    if(CuControlsReaderA* r = m_context->getReader())
        return r->source();
    return "";
}

void Qu_Reader::stop()
{
    if(CuControlsReaderA* r = m_context->getReader()) {
        r->unsetSource();
    }
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
