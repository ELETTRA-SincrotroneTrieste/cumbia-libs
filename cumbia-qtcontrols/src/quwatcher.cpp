#include "quwatcher.h"
#include <cumacros.h>
#include "qulabel.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QTimer>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"

class QuWatcherPrivate
{
public:
    CuContext *context;
};

QuWatcher::QuWatcher(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    Qumbiaizer(parent)
{
    d = new QuWatcherPrivate;
    d->context = new CuContext(cumbia, r_factory);

}

QuWatcher::QuWatcher(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    Qumbiaizer(parent)
{
    d = new QuWatcherPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
}

QuWatcher::~QuWatcher()
{
    pdelete("QuWatcher %p\n", this);
    delete d->context;
    delete d;
}

/** \brief returns the source of the watcher
 *
 * @return the source of the reader, if configured.
 *         An empty string otherwise.
 *
 * @see setSource
 * @see unsetSource
 */
QString QuWatcher::source() const
{
    if(CuControlsReaderA* r = d->context->getReader()) {
        printf("QuWatcher:source returning source reader %p \n ", r);
        return r->source();
    }
    return "";
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QuWatcher::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

/** \brief Stop the watcher
 *
 * Disposes the reader.
 *
 * @see setSource
 */
void QuWatcher::unsetSource()
{
    d->context->disposeReader();
    if(autoDestroy())
        deleteLater();
}

CuContext *QuWatcher::getContext() const
{
    return d->context;
}

void QuWatcher::onUpdate(const CuData &data)
{
    bool ok = !data["err"].toBool();
    std::string msg = data["msg"].toString();
    !ok ? d->context->getLinkStats()->addError(msg) : d->context->getLinkStats()->addOperation();

    if(data["type"].toString() == "property") {
        configure(data); // Qumbiaizer virtual configure method
        emit connectionOk(ok);
    }
    else if(data.containsKey("value")) {
        updateValue(data);
        if(singleShot()) {
            unsetSource();
        }
    }

    emit readOk(ok);
    emit refreshMessage(QString::fromStdString(msg));
    emit newData(data);
}
