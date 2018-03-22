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
    pdelete("QuWatcher %p", this);
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
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
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
}

CuContext *QuWatcher::getContext() const
{
    return d->context;
}

void QuWatcher::onUpdate(const CuData &data)
{
    if(data.containsKey("property"))
        configure(data); // Qumbiaizer virtual configure method
    else
    {
        updateValue(data);
        if(singleShot())
            unsetSource();
        if(this->autoDestroy())
        {
            pinfo("auto destroying QuWatcher for \"%s\"", qstoc(source()));
            QTimer::singleShot(2000, this, SLOT(deleteLater()));
        }
    }
}
