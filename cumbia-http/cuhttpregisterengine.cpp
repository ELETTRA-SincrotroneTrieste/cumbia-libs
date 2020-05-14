#include "cuhttpregisterengine.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpcontrolswriter.h"
#include "cuhttptangoreplacewildcards.h"
#include "cuhttptangohelper.h"

#include <cucontrolsfactorypool.h>
#include <cumbiahttpworld.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>


class CuHttpRegisterEnginePrivate {
public:
    QString url;
};

CuHttpRegisterEngine::CuHttpRegisterEngine() {
    d = new CuHttpRegisterEnginePrivate;
    d->url = "http://localhost:12702";
}

CuHttpRegisterEngine::~CuHttpRegisterEngine() {
    delete d;
}

CumbiaHttp *CuHttpRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo)
{
    // setup Cumbia http with the http address and the host name to prepend to the sources
    // for the HTTP requests
    CumbiaHTTPWorld httpw;
    CumbiaHttp* cuhttp = new CumbiaHttp(url(), new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("http", cuhttp);
    fpoo.registerImpl("http", CuHTTPReaderFactory());
    fpoo.registerImpl("http", CuHttpControlsWriterFactory());
    // example source: "http://tango://hokuto:20000/test/device/1/double_scalar"
    // http:// domain prefix will be discarded
    //
    // case 1: http specific app
    //
    // m_ctrl_factory_pool.setSrcPatterns("http", httpw.srcPatterns());
    // cu_pool->setSrcPatterns("http", httpw.srcPatterns());
    //
    // case 2: access Tango through a http server
    //
    // Share the same source syntax (e.g. $1/attribute, my/tango/dev/attribute)
    // across native tango engine and http proxy server.
    // This allohttp to leave the application code unchanged. See (*) below
    //
    CuHttpTangoReplaceWildcards *tgrwi = new CuHttpTangoReplaceWildcards(/*QStringList() << "test/device/1"*/);
    cuhttp->addReplaceWildcardI(tgrwi);
    CuHttpTangoHelper th;
    fpoo.setSrcPatterns("http", th.srcPatterns());
    cu_pool->setSrcPatterns("http", th.srcPatterns());
    return cuhttp;
}

void CuHttpRegisterEngine::setUrl(const QString &url) {
    d->url = url;
}

QString CuHttpRegisterEngine::url() const {
    return d->url;
}

bool CuHttpRegisterEngine::hasCmdOption(QCommandLineParser *parser, const QStringList &args) const {
    QCommandLineOption http_url_o(QStringList() << "u" << "http-url", "URL to http server", "url", url());
    if(!parser->optionNames().contains("u")) {
        parser->addOption(http_url_o);
    }
    parser->process(args);
    d->url = parser->value(http_url_o);
    return parser->isSet(http_url_o) && (d->url.startsWith("http://") || d->url.startsWith("https://"));
}
