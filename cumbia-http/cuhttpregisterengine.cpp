#include "cuhttpregisterengine.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpcontrolswriter.h"
#include "cuhttptangoreplacewildcards.h"
#include "cuhttptangohelper.h"
#include "cuhttptangosrchelper.h"

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
    int ttl;
    QCommandLineParser parser;
};

CuHttpRegisterEngine::CuHttpRegisterEngine() {
    d = new CuHttpRegisterEnginePrivate;
    d->ttl = 5;
}

CuHttpRegisterEngine::~CuHttpRegisterEngine() {
    delete d;
}

/*!
 * \brief Setup CumbiaHttp with some default options
 * \param cu_pool a pointer to a valid CumbiaPool. A new CumbiaHttp instance will be registered with name "http"
 * \param fpoo a reference to a CuControlsFactoryPool that will be initialized with registerImpl, name "http"
 * \return a pointer to a newly allocated CumbiaHttp registered into cu_pool with name "http"
 *
 * \par Default operations
 * The following helpers are installed on CumbiaHttp:
 * \li CuHttpTangoReplaceWildcards to replace wildcards in the command line arguments as the native tango engine
 *     would do;
 * \li CuHttpTangoHelper is used to get Tango source patterns
 * \li CuHttpTangoSrcHelper is registered to CumbiaHttp so that attribute and command readings suitable to be
 *     monitored over time can be distinguished from single-shot database operations and the TANGO_HOST is
 *     prepended to the source name if missing. See CuHttpTangoSrcHelper::setOperation
 */
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
    // if a Tango source is forwarded to the http/SSE service without the host information, the service may not
    // be able to connect. CuHttpTangoSrcHelper comes in prepending the host to the source, if missing
    CuHttpTangoSrcHelper *tg_src_h = new CuHttpTangoSrcHelper();
    tg_src_h->setOperation(CuHttpTangoSrcHelper::PrependHost, true);
    cuhttp->addSrcHelper(tg_src_h);
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

/*!
 * \brief load the http engine: parse the arguments and set the URL, channel, ttl, if possible either from args or
 *        cumbia module-default user settings
 * \param args the command line arguments
 * \param force_load if true, try to load the engine also if args does not contain "-u" (for example, if an URL
 *        is configured by `cumbia module-default` and we are hot switching the engine to http at runtime)
 * \return true if loading is successful ('-u' or force_load *and* it was possible to get an URL from either '-u'
 *         option or the user settings)
 */
bool CuHttpRegisterEngine::load(const QStringList &args, bool force_load) const {
    QCommandLineOption http_url_o(QStringList() << "u" << "http-url", "URL to http server/channel or URL only if -c [--channel] is provided", "url");
    QCommandLineOption chan_o(QStringList() << "c" << "channel", "Server Sent Events channel name", "chan");
    QCommandLineOption native_o(QStringList()  << "n" << "native", "Prefer native module");
    QCommandLineOption ttl_o(QStringList()  << "ttl" << "chan-msgs-ttl" , "Time to live: discard messages from the channel older than this value [seconds]", "integer");;
    d->parser.addOption(http_url_o);
    d->parser.addOption(chan_o);
    d->parser.addOption(native_o);
    d->parser.addOption(ttl_o);
    d->parser.addHelpOption();
    d->parser.parse(args);
    QString url;
    if(d->parser.isSet("help"))
        printf("http module \e[1;32mhelp\e[0m:\n\e[1;36m%s\e[0m\n", qstoc(d->parser.helpText()));
    if(d->parser.isSet(native_o))
        return false;
    if(d->parser.isSet(http_url_o))
        url = d->parser.value(http_url_o);
    if(d->parser.isSet(ttl_o))
        d->ttl = d->parser.value(ttl_o).toInt();
    if(d->parser.isSet(chan_o))
        d->chan = d->parser.value(chan_o);
    else if(!url.isEmpty()) {
        d->chan = url.section(QRegularExpression("[^:^/]/"), -1); // match last token after a / but skip http[s]://
        d->chan != url ? d->url = url.remove(url.lastIndexOf('/'), d->chan.length() + 1) : d->chan.remove(0, d->chan.length());
        if(d->url.contains(QRegularExpression("http[s]{0,1}://.*")) && d->chan.isEmpty())
            printf("\e[1;33m*\e[0m CuHttpRegisterEngine: channel not detected in URL: required form: \"%s/\e[1;33mchannel_name\e[0m\"\n",
                   qstoc(d->url));
    }

    if(url.isEmpty()) { // d->chan is set only if specified in -c option at this point
        bool oldv, http_def = httpModeDefault(&oldv); // oldv true: old file format (< 1.5.0)
        if(oldv) {
            print_upgrade();
        }
        if(http_def)
            url = urlDefault(oldv);
        else if((force_load || d->parser.isSet(http_url_o)) && !oldv) // -u set without URL: OK in >= 1.5.0
            url = urlDefault(false);
    }
    if(!url.isEmpty()) {
        d->url = url;
        if(d->chan.isEmpty())   // hash of app name and cmd line args
            d->chan = m_make_hash(args);
    }
    bool http_mod = !d->url.isEmpty() && d->chan.size() > 0 && (d->url.startsWith("http://") || d->url.startsWith("https://"));
    return http_mod;
}

/*! for cumbia < 1.5.0 compatibility. calls load(args)
 */
bool CuHttpRegisterEngine::hasCmdOption(const QStringList &args) const {
    return load(args, false);
}

bool CuHttpRegisterEngine::httpModeDefault(bool *oldv) const {
    QString l;
    QString cfgf = QDir::homePath() + QString("/%1/modules").arg(CUMBIA_USER_CONFIG_DIR);
    QFile f(cfgf);
    if(f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while(!in.atEnd() && (!l.startsWith("url:") && l != "http")) {
            l = in.readLine();
        }
        f.close();
    }
    // url: support old style
    // http: since 1.5.0
    if(oldv)
        *oldv = l.startsWith("url:");
    return l.contains("url:") || l == "http";
}

QString CuHttpRegisterEngine::urlDefault(bool oldv) const
{
    QString url;
    QString fnam = oldv ? "modules" : "url";
    QString cfgf = QDir::homePath() + QString("/%1/%2").arg(CUMBIA_USER_CONFIG_DIR).arg(fnam);
    QFile f(cfgf);
    if(f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while(!in.atEnd() && url.isEmpty()) {
            QString l = in.readLine();
            if(l.startsWith("url:"))
                url = l.remove("url:");
            else if(l.startsWith("http"))
                url = l;
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

void CuHttpRegisterEngine::print_upgrade() const {
    printf("\n\033[1;33m*\n*\033[1;31m old cumbia 'module-default' configuration file detected:\n");
    printf("\033[1;33m*\033[1;31m please execute\n   \033[0m cumbia apps module default [set]\n");
    printf("\033[1;33m*\033[1;31m on the command line\033[1;33m*\033[0m\n\n");
}
