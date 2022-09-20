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
    void setChannel(const QString& chan);
    QString url() const;
    QString channel() const;
    bool load(const QStringList& args, bool force_load = false) const;
    bool hasCmdOption(const QStringList &args) const; // < 1.5.0 compat

    QString urlDefault(bool oldv = false) const;
    bool httpModeDefault(bool *oldv) const;


private:
    CuHttpRegisterEnginePrivate *d;
    QByteArray m_make_hash(const QStringList& args) const;
    void print_upgrade() const;
};

#endif // CUHTTPREGISTERENGINE_H
