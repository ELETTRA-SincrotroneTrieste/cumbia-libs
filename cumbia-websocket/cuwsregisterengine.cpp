#include "cuwsregisterengine.h"
#include "cumbiawebsocket.h"
#include "cuwscontrolsreader.h"
#include "cuwscontrolswriter.h"
#include "cuwstangoreplacewildcards.h"
#include "cuwstangohelper.h"

#include <cucontrolsfactorypool.h>
#include <cumbiawsworld.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>


class CuWsRegisterEnginePrivate {
public:
    QString url;
};

CuWsRegisterEngine::CuWsRegisterEngine() {
    d = new CuWsRegisterEnginePrivate;
    d->url = "ws://localhost:12702";
}

CuWsRegisterEngine::~CuWsRegisterEngine() {
    delete d;
}

CumbiaWebSocket *CuWsRegisterEngine::registerWithDefaults(CumbiaPool *cu_pool, CuControlsFactoryPool &fpoo)
{
    // setup Cumbia web socket with the web socket address and the host name to prepend to the sources
    // for the HTTP requests
    CumbiaWSWorld wsw;
    CumbiaWebSocket* cuws = new CumbiaWebSocket(url(), "", new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("ws", cuws);
    fpoo.registerImpl("ws", CuWSReaderFactory());
    fpoo.registerImpl("ws", CuWsControlsWriterFactory());
    // example source: "ws://tango://hokuto:20000/test/device/1/double_scalar"
    // ws:// domain prefix will be discarded
    //
    // case 1: websocket specific app
    //
    // m_ctrl_factory_pool.setSrcPatterns("ws", wsw.srcPatterns());
    // cu_pool->setSrcPatterns("ws", wsw.srcPatterns());
    //
    // case 2: access Tango through a websocket server
    //
    // Share the same source syntax (e.g. $1/attribute, my/tango/dev/attribute)
    // across native tango engine and websocket proxy server.
    // This allows to leave the application code unchanged. See (*) below
    //
    CuWsTangoReplaceWildcards *tgrwi = new CuWsTangoReplaceWildcards(/*QStringList() << "test/device/1"*/);
    cuws->addReplaceWildcardI(tgrwi);
    CuWsTangoHelper th;
    fpoo.setSrcPatterns("ws", th.srcPatterns());
    cu_pool->setSrcPatterns("ws", th.srcPatterns());
    return cuws;
}

void CuWsRegisterEngine::setUrl(const QString &url) {
    d->url = url;
}

QString CuWsRegisterEngine::url() const {
    return d->url;
}

bool CuWsRegisterEngine::hasCmdOption(QCommandLineParser *parser, const QStringList &args) const
{
    QCommandLineOption ws_url_o(QStringList() << "u" << "websocket-url", "URL to websocket server", "url", url());
    parser->addOption(ws_url_o);
    parser->process(args);
    return parser->isSet(ws_url_o) && !parser->value(ws_url_o).isEmpty();
}
