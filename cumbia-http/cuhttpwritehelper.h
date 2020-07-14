#ifndef CUHTTPACTIONWRITER_H
#define CUHTTPACTIONWRITER_H

#include <cuhttpactiona.h>
#include "cuhttpauthmanager.h"
#include "cuhttpsrcman.h"

class CuHTTPWriteHelperPrivate;
class QNetworkReply;
class QNetworkAccessManager;
class CuHTTPSrc;
class CuHttpAuthManager;

class CuHttpWriteHelper : public QObject
{
    Q_OBJECT
public:
    CuHttpWriteHelper(QObject *parent = nullptr);
    ~CuHttpWriteHelper();

    CuData makeErrData(const QString &msg);

private:
    CuHTTPWriteHelperPrivate *d;
};

#endif // CUHTTPACTIONWRITER_H
