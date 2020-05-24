#include "cuhttpregisterengine.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpcontrolswriter.h"
#include "cuhttptangoreplacewildcards.h"
#include "cuhttptangohelper.h"

#include <QRegularExpression>
#include <cucontrolsfactorypool.h>
#include <cumbiahttpworld.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>


class CuHttpRegisterEnginePrivate {
public:
    QString url, chan;
};

CuHttpRegisterEngine::CuHttpRegisterEngine() {
    d = new CuHttpRegisterEnginePrivate;
    d->url = "http://localhost:12702";
    d->chan = "1";
}

CuHttpRegisterEngine::~CuHttpRegisterEngine() {
    delete d;
}

CumbiaHttp *CuHttpRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo)
{
    // setup Cumbia http with the http address and the host name to prepend to the sources
    // for the HTTP requests
    CumbiaHTTPWorld httpw;
    CumbiaHttp* cuhttp = new CumbiaHttp(url(), channel(), new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
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

void CuHttpRegisterEngine::setChannel(const QString &chan) {
    d->chan = chan;
}

QString CuHttpRegisterEngine::url() const {
    return d->url;
}

QString CuHttpRegisterEngine::channel() const {
    return d->chan;
}

bool CuHttpRegisterEngine::hasCmdOption(QCommandLineParser *parser, const QStringList &args) const {
    QCommandLineOption http_url_o(QStringList() << "u" << "http-url", "URL to http server/channel", "url", url());
    if(!parser->optionNames().contains("u")) {
        parser->addOption(http_url_o);
    }
    parser->process(args);
    QString url = parser->value(http_url_o);
    d->chan = url.section(QRegularExpression("[^:^/]/"), -1); // match last token after a / but skip http[s]://
    d->chan != url ? d->url = url.remove(url.lastIndexOf('/'), d->chan.length() + 1) : d->chan.remove(0, d->chan.length());
    if(d->url.contains(QRegularExpression("http[s]{0,1}://.*")) && d->chan.isEmpty())
         printf("\e[1;33m*\e[0m CuHttpRegisterEngine: channel not detected in URL: required form: \"%s/\e[1;33mchannel_name\e[0m\"\n",
                qstoc(d->url));
    return !d->url.isEmpty() && d->chan.size() > 0 && (d->url.startsWith("http://") || d->url.startsWith("https://"));
}
