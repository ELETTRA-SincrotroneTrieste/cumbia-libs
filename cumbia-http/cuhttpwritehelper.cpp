#include "cuhttpwritehelper.h"

class CuHTTPWriteHelperPrivate
{
public:

     QList<SrcItem> tgtdata;
};

CuHttpWriteHelper::CuHttpWriteHelper(QObject *parent) : QObject(parent) {
    d = new CuHTTPWriteHelperPrivate;
}

CuHttpWriteHelper::~CuHttpWriteHelper() {
    pdelete("~CuHttpWriteHelper %p", this);
    delete d;
}

CuData CuHttpWriteHelper::makeErrData(const QString &msg) {
    CuData da("msg", msg.toStdString());
    da.set("err", true).putTimestamp();
    return da;
}
