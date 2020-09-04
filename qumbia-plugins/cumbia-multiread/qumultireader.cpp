#include "qumultireader.h"
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cudata.h>
#include <QTimer>
#include <QMap>
#include <QtDebug>

class QuMultiReaderPrivate
{
public:
    QStringList srcs;
    QMap<QString, CuControlsReaderA* > readersMap;
    bool sequential;
    int period, manual_mode_code;
    CuContext *context;
    QTimer *timer;
    QList<CuData> databuf;
};

QuMultiReader::QuMultiReader(QObject *parent) :
    QObject(parent)
{
    d = new QuMultiReaderPrivate;
    d->sequential = false;
    d->period = 1000;
    d->manual_mode_code = -1; // parallel reading
    d->timer = NULL;
    d->context = NULL;
}

QuMultiReader::~QuMultiReader()
{
    if(d->context)
        delete d->context;
    delete d;
}

void QuMultiReader::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, int manual_mode_code)
{
    d->context = new CuContext(cumbia, r_fac);
    d->manual_mode_code = manual_mode_code;
    d->sequential = d->manual_mode_code >= 0;
}

void QuMultiReader::init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, int manual_mode_code)
{
    d->context = new CuContext(cumbia_pool, fpool);
    d->manual_mode_code = manual_mode_code;
    d->sequential = (d->manual_mode_code >= 0);
}

void QuMultiReader::setSources(const QStringList &srcs)
{
    unsetSources();
    for(int i = 0; i < srcs.size(); i++)
        insertSource(srcs[i], i);
}

void QuMultiReader::unsetSources()
{
    d->context->disposeReader(); // empty arg: dispose all
    d->srcs.clear();
    d->readersMap.clear();
}

/** \brief inserts src at index position i in the list. If i <= 0, src is prepended to the list. If i >= size(),
 * src is appended to the list, whether or not src is already there.
 *
 * @see setSources
 */
void QuMultiReader::insertSource(const QString &src, int i)
{
    CuData options;
    if(d->sequential)  {
        options["period"] = d->period;
        options["refresh_mode"] = d->manual_mode_code;
        options["manual"] = true;
        d->context->setOptions(options);
    }
    CuControlsReaderA* r = d->context->add_reader(src.toStdString(), this);
    if(r) {
        r->setSource(src); // then use r->source, not src
        d->readersMap.insert(r->source(), r);
        d->srcs.insert(i, r->source());
    }

    if(d->srcs.size() == 1 && d->sequential)
        m_startTimer();
}

void QuMultiReader::removeSource(const QString &src)
{
    if(d->context)
        d->context->disposeReader(src.toStdString());
    d->srcs.removeAll(src);
    d->readersMap.remove(src);
}

/** \brief returns a reference to this object, so that it can be used as a QObject
 *         to benefit from signal/slot connections.
 *
 */
const QObject *QuMultiReader::get_qobject() const
{
    return this;
}

QStringList QuMultiReader::sources() const
{
    return d->srcs;
}

int QuMultiReader::period() const
{
    return d->period;
}

void QuMultiReader::setPeriod(int ms) {
    d->period = ms;
    if(!d->sequential)
    {
        CuData per("period", d->period);
        foreach(CuControlsReaderA *r, d->context->readers())
            r->sendData(per);
    }
}

void QuMultiReader::setSequential(bool seq) {
    d->sequential = seq;
}

bool QuMultiReader::sequential() const {
    return d->sequential;
}

void QuMultiReader::startRead()
{
    if(d->srcs.size() > 0)
        d->readersMap[d->srcs.first()]->sendData(CuData("read", ""));

    if(d->srcs.size() > 0)
        printf("QuMultiReader.startRead: started cycle with read command for %s...\n", qstoc(d->srcs.first()));
}

void QuMultiReader::m_startTimer()
{
    if(!d->timer) {
        d->timer = new QTimer(this);
        connect(d->timer, SIGNAL(timeout()), this, SLOT(startRead()));
        d->timer->setSingleShot(true);
        d->timer->start(d->period);
        printf("+ timer started with period %d\n", d->period);
    }
}

void QuMultiReader::onUpdate(const CuData &data)
{
    QString from = QString::fromStdString( data["src"].toString());
    int pos = d->srcs.indexOf(from);
    printf("QuMultiReader::onUpdate got data from %s [%d/%d]...\n", qstoc(from), pos, d->srcs.size());
    if(pos >= 0) {
        emit onNewData(data);

        if(d->sequential)
            d->databuf.append(data);

        if(d->sequential && ++pos < d->srcs.size()) {
            d->readersMap[d->srcs[pos]]->sendData(CuData("read", ""));
        }
        else if(d->sequential) {
            // one read cycle is over: emit signal
            emit onSeqReadComplete(d->databuf);
            d->databuf.clear();
            d->timer->start(d->period);
            printf("+ read cycle complete, restarting timer, timeout %d\n", d->timer->interval());
        }
    }
    else
        perr("QuMultiReader.onUpdate: source \"%s\" not found", qstoc(from));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cumbia-multiread, QuMultiReader)
#endif // QT_VERSION < 0x050000
