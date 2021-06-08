#include "cuhttpchannelreceiver.h"
#include "cumbiahttpworld.h"
#include <cudatalistener.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class CuHttpChannelReceiverPrivate {
public:
    QMultiMap<QString, CuDataListener *> rmap;
    QString url, chan;
    time_t data_exp_t;
    bool exit;
};

CuHttpChannelReceiver::CuHttpChannelReceiver(const QString &url,
                                             const QString &chan,
                                             QNetworkAccessManager *nam) : CuHTTPActionA(nam) {
    d = new CuHttpChannelReceiverPrivate;
    d->url = url;
    d->chan = chan;
    d->data_exp_t = DEFAULT_CHAN_MSG_TTL; // after these seconds data is old
    d->exit = false;
}

QString CuHttpChannelReceiver::channel() const {
    return d->chan;
}

/*!
 * \brief set the number of seconds after which data from the channel is considered old and discarded
 *
 * \par Note
 * The timestamp stored in the received data is compared to the current system time in order to
 * determine whether it is expired or still valid.
 *
 * \par Default value
 * DEFAULT_CHAN_MSG_TTL value configured in cumbia-http.pro (seconds)
 */
void CuHttpChannelReceiver::setDataExpireSecs(time_t secs) {
    d->data_exp_t = secs;
}

/*!
 * \brief returns the number of seconds after which data received from the channel is discarded
 * \
 * @see setDataExpireSecs
 */
time_t CuHttpChannelReceiver::dataExpiresSecs() const {
    return d->data_exp_t;
}

QString CuHttpChannelReceiver::getSourceName() const {
    QString channel = d->url + "/sub/" + d->chan;
    return channel;
}

CuHTTPActionA::Type CuHttpChannelReceiver::getType() const {
    return CuHTTPActionA::ChannelReceiver;
}

void CuHttpChannelReceiver::addDataListener(const QString &src, CuDataListener *l) {
    d->rmap.insert(src, l);
}

void CuHttpChannelReceiver::addDataListener(CuDataListener *l) { }

void CuHttpChannelReceiver::removeDataListener(CuDataListener *l) {
    QMutableMapIterator<QString, CuDataListener* > it(d->rmap);
    while(it.hasNext()) {
        it.next();
        if(it.value() == l) {
            it.remove();
        }
    }
}

size_t CuHttpChannelReceiver::dataListenersCount() {
    return d->rmap.size();
}

void CuHttpChannelReceiver::start() {
    QString chan = d->url + "/sub/" + d->chan;
    cuprintf("CuHttpChannelReceiver.start: listening on channel \e[0;32m%s\e[0m\n", qstoc(chan));
    startRequest(chan);
}

bool CuHttpChannelReceiver::exiting() const {
    return d->exit;
}

void CuHttpChannelReceiver::stop() {
    cuprintf("CuHttpChannelReceiver::stop: \e[1;31mcheck me!\e[0m\n");
    d->exit = true;
    stopRequest();
}

void CuHttpChannelReceiver::decodeMessage(const QJsonValue &v) {
    const QJsonArray a = v.toArray(); // data arrives within an array
    if(a.size() == 1) {
        const QString& src = a.at(0)["src"].toString();
        qDebug() << __PRETTY_FUNCTION__  << v << "rmap contains? " << d->rmap.contains(src) << d->rmap;
//        double t_ms = a.at(0)["timestamp_ms"].toDouble();
        time_t diff_t;
//        bool data_fresh = m_data_fresh(t_ms, &diff_t);
        bool data_fresh = true;

        if(d->rmap.contains(src) && data_fresh) {
            CuData res("src", src.toStdString());
            CumbiaHTTPWorld httpw;
            httpw.json_decode(a.at(0), res);
            printf("CuHttpChannelReceiver.decodeMessage \e[1;33m to %s\e[0m\n", datos(res));
            foreach(CuDataListener *l, d->rmap.values(src)) {
                l->onUpdate(res);
            }
        }
        else if(!data_fresh) {
            CuData res("src", src.toStdString());
            CumbiaHTTPWorld httpw;
            httpw.json_decode(a.at(0), res);
            perr("CuHttpChannelReceiver::decodeMessage: source \"%s\" data is too old: %lds > %lds",
                 qstoc(src), diff_t, d->data_exp_t);
        }
    }
}

QNetworkRequest CuHttpChannelReceiver::prepareRequest(const QUrl &url) const
{
    /*
     * -- sniffed from JS EventSource -- tcpdump -vvvs 1024 -l -A -i lo port 8001 -n
     * .^...^..GET /sub/subscribe/hokuto:20000/test/device/1/double_scalar HTTP/1.1
        Host: woody.elettra.eu:8001
        User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:76.0) Gecko/20100101 Firefox/76.0
        Accept: text/event-stream
        Accept-Language: en-US,en;q=0.5
        Accept-Encoding: gzip, deflate
        Origin: http://woody:8001
        DNT: 1
        Connection: keep-alive
        Referer: http://woody:8001/
        Pragma: no-cache
        Cache-Control: no-cache
    */
    QNetworkRequest r (url);
    r = CuHTTPActionA::prepareRequest(url);
    r.setRawHeader("Accept", "text/event-stream");
    r.setRawHeader("Accept-Encoding", "gzip, deflate");
    r.setRawHeader("Connection", "keep-alive");
    r.setRawHeader("Pragma", "no-cache");
    r.setRawHeader("Cache-Control", "no-cache");

    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    r.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached
    return r;
}

bool CuHttpChannelReceiver::m_data_fresh(const double timestamp_ms, time_t* diff_t) const {
    long ts_sec = static_cast<long>(timestamp_ms / 1000.0);
    time_t now;
    time(&now);
    char _now[256], _then[256];
    strcpy(_now, ctime(&now));
    strcpy(_then, ctime(&ts_sec));
    *diff_t = now - ts_sec;
//        printf("CaCuTangoEImpl data ts %s, data now %s now -ts_sec %ld < %ld? %s\n",
//               _then, _now, now - ts_sec, d->data_exp_t, (*diff_t < d->data_exp_t) ? "\e[1;32myes\e[0m" : "\e[1;31mno\e[0m");
//        if(*diff_t >= d->data_exp_t)
//            cuprintf("CaCuTangoEImpl.m_data_valid \n\e[1;35m%ld %s -  %ld %s < 3 sec ? %ld\e[0m\n",
//                      now, _now, ts_sec, _then, *diff_t);
    return *diff_t < d->data_exp_t;
}

