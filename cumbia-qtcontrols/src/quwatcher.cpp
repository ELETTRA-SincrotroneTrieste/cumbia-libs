#include "quwatcher.h"
#include <cumacros.h>
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QTimer>
#include <quapplication.h>
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontext.h"
#include "cuengine_swap.h"

class QuWatcherPrivate
{
public:
    CuContext *context;
};

QuWatcher::QuWatcher(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    Qumbiaizer(parent) {
    d = new QuWatcherPrivate;
    d->context = new CuContext(cumbia, r_factory);

}

QuWatcher::QuWatcher(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    Qumbiaizer(parent) {
    d = new QuWatcherPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
}

/*!
 * \brief Simplified constructor with parent object and optional source and options
 * \param parent the parent object
 * \param source the source of data
 * \param target the target object on which the *slot* shall be invoked
 * \param slot the *SLOT* to be invoked
 * \param options CuContext options
 * \param setPointSlot the slot to be invoked when the "set point" is available (engine and data dependent)
 * \param connType the Qt connection type (default: Qt::AutoConnection)
 *
 * \note Requires *QuApplication*
 *
 * If *source, target and slot* are *all* specified, the object is connected immediately and will start calling *slot*
 * target, slot, (and optional setPointSlot and connType) are passed to the *attach* method
 *
 * This constructor is a single liner that stands for
 * \code
 * QuWatcher *w = new QuWatcher(parent);
 * if(target && slot && !source.isEmpty()) {
 *      w->attach(target, slot, setPointSlot, connType);
 *      w->getContext()->setOptions(options);
 *      attach(target, slot, setPointSlot, connType);
        setSource(source);
 * }
 * \endcode
 *
 * \par Example
 * \code
 * QuWatcher *w = new QuWatcher(this, "$1/current", this, SLOT(update(const CuData&)));
 * \endcode
 *
 * \since 2.1
 */
QuWatcher::QuWatcher(QObject *parent,
                     const QString& source,
                     QObject *target,
                     const char *slot,
                     const CuData &options,
                     const char *setPointSlot,
                     Qt::ConnectionType connType)  :
    Qumbiaizer(parent) {
    d = new QuWatcherPrivate;
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    d->context = new CuContext(a->cumbiaPool(), *a->fpool());
    if(!source.isEmpty() && target && slot) {
        if(!options.isEmpty())
            d->context->setOptions(options);
        attach(target, slot, setPointSlot, connType);
        setSource(source);
    }
}

QuWatcher::~QuWatcher() {
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
        d->context->setOptions(d->context->options().set("single-shot", true));
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

bool QuWatcher::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

CuContext *QuWatcher::getContext() const {
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
    bool ok = !data[TTT::Err].toBool();
    bool is_config = data.has(TTT::Type, "property");
    const char *msg = data[TTT::Message].c_str();
    if(is_config) {
        configure(data); // Qumbiaizer virtual configure method
    }
    if(data.containsKey(TTT::Value))
        updateValue(data);
    if(singleShot())
        unsetSource();

    // newData flavored signals
    // are emitted from Qumbiaizer::updateValue
    // generic newData signal is emitted here
    emit readOk(ok);
    if(msg != nullptr)
        emit refreshMessage(msg);
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

Qu1TWatcher::Qu1TWatcher(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QuWatcher(parent, cumbia, r_factory) {
    setSingleShot(true);
    setAutoDestroy(true);
}

Qu1TWatcher::~Qu1TWatcher() {

}
