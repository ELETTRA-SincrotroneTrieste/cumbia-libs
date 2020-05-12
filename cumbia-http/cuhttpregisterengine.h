#ifndef CUHTTPREGISTERENGINE_H
#define CUHTTPREGISTERENGINE_H

#include <QStringList>
#include <QCommandLineParser>
#include <cumbiapool.h>
#include <cumbiahttp.h>

class CumbiaPool;
class CuControlsFactoryPool;

class CuHttpRegisterEnginePrivate;

class CuHttpRegisterEngine
{
public:
    CuHttpRegisterEngine();

    virtual ~CuHttpRegisterEngine();

    CumbiaHttp *registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo);

    void setUrl(const QString& url);

    QString url() const;

    bool hasCmdOption(QCommandLineParser *parser, const QStringList& args) const;

private:
    CuHttpRegisterEnginePrivate *d;
};

#endif // CUHTTPREGISTERENGINE_H
