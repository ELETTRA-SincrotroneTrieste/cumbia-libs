#include "reader.h"
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontext.h>
#include <cudata.h>

Reader::Reader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QObject(parent)
{
    m_context = new CuContext(cumbia_pool, fpool);
}

Reader::~Reader()
{
    delete m_context;
}


void Reader::onUpdate(const CuData &data)
{

    const CuVariant&  v = data["value"];
    double ts = data["timestamp_us"].toDouble();
//    printf("%s\n", data.toString().c_str());
    QString src = QString::fromStdString(data["src"].toString());
    if(data["err"].toBool())
        emit newError(src, ts, QString::fromStdString(data["msg"].toString()));
    else {
        if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Double)
            emit newDouble(src, ts, v.toDouble());
        if(v.getFormat() == CuVariant::Scalar && v.getType() == CuVariant::Short)
            emit newShort(src, ts, v.toShortInt());
        else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Double)
            emit newDoubleVector(src, ts, QVector<double>::fromStdVector(v.toDoubleVector()));
        else if(v.getFormat() == CuVariant::Vector && v.getType() == CuVariant::Short)
            emit newShortVector(src, ts, QVector<short>::fromStdVector(v.toShortVector()));
    }
}

QString Reader::source() const
{
    if(CuControlsReaderA* r = m_context->getReader())
        return r->source();
    return "";
}

void Reader::setPeriod(int ms)
{
    m_context->sendData(CuData("period", ms));
}

void Reader::setSource(const QString &s)
{
    CuControlsReaderA * r = m_context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}
