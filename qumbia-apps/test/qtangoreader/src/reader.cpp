#include "reader.h"
#include <com_proxy_reader.h>

Reader::Reader(QObject *parent) : QObject(parent), QTangoComProxyReader(this)
{
    connect(qtangoComHandle(), SIGNAL(newData(TVariant)), this, SLOT(refresh(TVariant)));
}

Reader::~Reader()
{
}

void Reader::refresh(const TVariant &v)
{
    struct timeval tv = v.timeRead();
    double ts = tv.tv_sec + tv.tv_usec * 1e-6;
    const QString src = v.tangoPoint();
    if(v.quality() == ATTR_INVALID)
        emit newError(src, ts, v.message());
    else{
        if(v.canConvertToDouble())
            emit newDouble(src, ts, v.toDouble());
        if(v.canConvertToInt())
            emit newInt(src, ts, v.toInt());
        else if(v.canConvertToDoubleVector())
            emit newDoubleVector(src, ts, v.toDoubleVector());
        else if(v.canConvertToIntVector())
            emit newIntVector(src, ts, v.toIntVector());
    }
}
