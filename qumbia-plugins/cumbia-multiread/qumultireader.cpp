#include "qumultireader.h"
#include <cucontext.h>
#include <cucontrolsreader_abs.h>

class QuMultiReaderPrivate
{
public:
    QStringList srcs;
    bool sequential;
    int period;
    CuContext *context;
};

QuMultiReader::QuMultiReader(QObject *parent) :
    QObject(parent)
{
    d = new QuMultiReaderPrivate;
    d->sequential = false;
    d->period = 1000;
    d->context = NULL;
}

QuMultiReader::~QuMultiReader()
{
    if(d->context)
        delete d->context;
    delete d;
}

void QuMultiReader::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    d->context = new CuContext(cumbia, r_fac);
}

void QuMultiReader::init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
{
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuMultiReader::setSources(const QStringList &srcs, bool sequential)
{
    d->sequential = sequential;
    unsetSources();
    if(sequential)
    {
        CuData opt("period", d->period);
        opt("")
        d->context->setOptions(opt);
    }
    foreach(QString src, srcs) {
        CuControlsReaderA* r = d->context->add_reader(src, this);
        if(sequential)
        {
        }
}

void QuMultiReader::unsetSources()
{
    d->context->disposeReader(); // empty arg: dispose all
}

void QuMultiReader::addSource(const QString &src, int after)
{

}

void QuMultiReader::removeSource(const QString &src)
{
    if(d->context)
        d->context->disposeReader(src.toStdString());
}

QStringList QuMultiReader::sources() const
{
    QStringList l;
    foreach(CuControlsReaderA *r, d->context->readers())
        l.append(r->source());
    return l;
}

int QuMultiReader::period() const
{
    return d->period;
}

void QuMultiReader::setPeriod(int ms)
{
    d->period = ms;
    CuData per("period", d->period);
    foreach(CuControlsReaderA *r, d->context->readers())
        r->sendData(per);
}

void QuMultiReader::onUpdate(const CuData &data)
{

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cumbia-multiread, MultiReader)
#endif // QT_VERSION < 0x050000
