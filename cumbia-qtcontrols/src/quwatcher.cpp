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
    if(singleShot())
        d->context->setOptions(CuData("single-shot", true));
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

/*! \brief QuWatcher implementation of the onUpdate method
 *
 * @param data the data received from the reader
 *
 * If data contains the *property* key, Qumbiaizer::configure is called and the
 * *configured* signal is whence emitted.
 * If data contains the *value* key and *is not a property type*, Qumbiaizer::updateValue is called.
 * From there, *type specific* signals are emitted and the configure slots are invoked.
 *
 * The following signals are always emitted at the end of this method:

 * \li newData(const CuData&)
 * \li readOk(bool ok)
 * \li refreshMessage(const QString& msg)
 *
 * Both the generic *configure* and *newData* signals carry the full data coming from the engine.
 *
 * @see Qumbiaizer::updateValue
 */
void QuWatcher::onUpdate(const CuData &data)
{
    bool ok = !data["err"].toBool();
    bool is_config = data.has("type", "property");
    std::string msg = data["msg"].toString();
    !ok ? d->context->getLinkStats()->addError(msg) : d->context->getLinkStats()->addOperation();

    if(is_config) {
        configure(data); // Qumbiaizer virtual configure method
    }
    if(data.containsKey("value")) {
        updateValue(data);
        if(singleShot()) {
            unsetSource();
        }
    }
    // newData flavored signals
    // are emitted from Qumbiaizer::updateValue
    // generic newData signal is emitted here
    emit readOk(ok);
    emit refreshMessage(QString::fromStdString(msg));
    emit newData(data);
}

/*!
 * \brief Qu1TWatcher::Qu1TWatcher one time auto destroying watcher
 * \param parent QObject parent of this object
 * \param cumbia_pool pointer to CumbiaPool
 * \param fpool const reference to CuControlsFactoryPool
 *
 * This is a convenience class whose constructor calls
 *
 * \li setSingleShot(true)
 * \li setAutoDestroy(true)
 */
Qu1TWatcher::Qu1TWatcher(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QuWatcher(parent, cumbia_pool, fpool) {
    setSingleShot(true);
    setAutoDestroy(true);
}

Qu1TWatcher::~Qu1TWatcher() {

}
