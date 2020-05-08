#ifndef CUWSREGISTERENGINE_H
#define CUWSREGISTERENGINE_H

#include <QStringList>
#include <QCommandLineParser>
#include <cumbiapool.h>
#include <cumbiawebsocket.h>

class CumbiaPool;
class CuControlsFactoryPool;

class CuWsRegisterEnginePrivate;

class CuWsRegisterEngine
{
public:
    CuWsRegisterEngine();

    virtual ~CuWsRegisterEngine();

    CumbiaWebSocket *registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo);

    void setUrl(const QString& url);

    QString url() const;

    bool hasCmdOption(QCommandLineParser *parser, const QStringList& args) const;

private:
    CuWsRegisterEnginePrivate *d;
};

#endif // CUWSREGISTERENGINE_H
