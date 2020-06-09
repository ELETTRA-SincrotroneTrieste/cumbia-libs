#include "cuhttpchannelreceiver.h"
#include "cuhttpactionreader.h"
#include <cudatalistener.h>
#include <QJsonDocument>
#include <QJsonObject>

class CuHttpChannelReceiverPrivate {
public:
    QMap<QString, CuHTTPActionReader *> rmap;
    QString url, chan;
    bool exit;
};

CuHttpChannelReceiver::CuHttpChannelReceiver(const QString &url,
                                             const QString &chan,
                                             QNetworkAccessManager *nam) : CuHTTPActionA(nam) {
    d = new CuHttpChannelReceiverPrivate;
    d->url = url;
    d->chan = chan;
    d->exit = false;
}

QString CuHttpChannelReceiver::channel() const {
    return d->chan;
}

void CuHttpChannelReceiver::registerReader(const QString &src, CuHTTPActionReader *r) {
    d->rmap.insert(src, r);
}

void CuHttpChannelReceiver::unregisterReader(const QString &src) {
    d->rmap.remove(src);
}

CuHTTPSrc CuHttpChannelReceiver::getSource() const {
    QString channel = d->url + "/sub/" + d->chan;
    return CuHTTPSrc(channel.toStdString());
}

CuHTTPActionA::Type CuHttpChannelReceiver::getType() const {
    return CuHTTPActionA::ChannelReceiver;
}

void CuHttpChannelReceiver::addDataListener(CuDataListener *) { }

void CuHttpChannelReceiver::removeDataListener(CuDataListener *) { }

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

void CuHttpChannelReceiver::decodeMessage(const QJsonDocument &json) {
    const QJsonObject data_o = json.object();
    const QString& src = data_o["src"].toString();
    double t_ms = data_o["timestamp_ms"].toDouble();
    if(d->rmap.contains(src) && m_data_fresh(t_ms))
        d->rmap.value(src)->decodeMessage(json);
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

bool CuHttpChannelReceiver::m_data_fresh(const double timestamp_ms) const {
    const long data_old = 1;
    long ts_sec = static_cast<long>(timestamp_ms / 1000.0);
    time_t now;
    time(&now);
//    char _now[256], _then[256];
//    strcpy(_now, ctime(&now));
//    strcpy(_then, ctime(&ts_sec));
//    printf("CaCuTangoEImpl data ts %s, data now %s now -ts_sec %ld < %ld? %s\n",
//           _then, _now, now - ts_sec, data_old, (now - ts_sec < data_old) ? "\e[1;32myes\e[0m" : "\e[1;31mno\e[0m");
//    if(now - ts_sec >= data_old)
//        cuprintf("CaCuTangoEImpl.m_data_valid \n\e[1;35m%ld %s -  %ld %s < 3 sec ? %ld\e[0m\n",
//                  now, _now, ts_sec, _then, (now - ts_sec));
    return now - ts_sec < data_old;
}

