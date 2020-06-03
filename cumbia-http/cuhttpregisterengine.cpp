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

#include <QDir>
#include <QTextStream>
#include <QFile>
#include <QCryptographicHash>

class CuHttpRegisterEnginePrivate {
public:
    QString url, chan;
};

CuHttpRegisterEngine::CuHttpRegisterEngine() {
    d = new CuHttpRegisterEnginePrivate;
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

bool CuHttpRegisterEngine::hasCmdOption(const QStringList &args) const {
    QCommandLineParser parser;
    QCommandLineOption http_url_o(QStringList() << "u" << "http-url", "URL to http server/channel or URL only if -c [--channel] is provided", "url");
    QCommandLineOption chan_o(QStringList() << "c" << "channel", "Server Sent Events channel name", "chan");
    parser.addOption(http_url_o);
    parser.addOption(chan_o);
    parser.parse(args);
    QString url;
    if(parser.isSet(http_url_o))
        url = parser.value(http_url_o);
    if(parser.isSet(chan_o))
        d->chan = parser.value(chan_o);
    else if(!url.isEmpty()) {
        d->chan = url.section(QRegularExpression("[^:^/]/"), -1); // match last token after a / but skip http[s]://
        d->chan != url ? d->url = url.remove(url.lastIndexOf('/'), d->chan.length() + 1) : d->chan.remove(0, d->chan.length());
        if(d->url.contains(QRegularExpression("http[s]{0,1}://.*")) && d->chan.isEmpty())
            printf("\e[1;33m*\e[0m CuHttpRegisterEngine: channel not detected in URL: required form: \"%s/\e[1;33mchannel_name\e[0m\"\n",
                   qstoc(d->url));
    }
    if(url.isEmpty()) // d->chan is set only if specified in -c option at this point
        url = urlFromConfig();
    if(!url.isEmpty()) {
        d->url = url;
        if(d->chan.isEmpty())   // hash of app name and cmd line args
            d->chan = m_make_hash(args);
    }
    bool http_mod = !d->url.isEmpty() && d->chan.size() > 0 && (d->url.startsWith("http://") || d->url.startsWith("https://"));
    return http_mod;
}

QString CuHttpRegisterEngine::urlFromConfig() const
{
    QString url;
    QString cfgf = QDir::homePath() + QString("/%1/modules").arg(CUMBIA_USER_CONFIG_DIR);
    QFile f(cfgf);
    if(f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while(!in.atEnd() && url.isEmpty()) {
            QString l = in.readLine();
            if(l.startsWith("url:"))
                url = l.remove("url:");
        }
        f.close();
    }
    return url;
}

QByteArray CuHttpRegisterEngine::m_make_hash(const QStringList &args) const
{
    QCryptographicHash cryha(QCryptographicHash::Md5);
    for(int i = 1; i < args.size(); i++)
        cryha.addData(args[i].toLocal8Bit());
    return args[0].section('/', -1).toLocal8Bit() + '-' + cryha.result().toHex();
}
