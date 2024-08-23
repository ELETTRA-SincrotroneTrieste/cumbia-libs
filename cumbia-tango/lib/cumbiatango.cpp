#include "cumbiatango.h"
#include <cumbia.h>
#include <culog.h>
#include <cumacros.h>
#include <cudatalistener.h>

#include <cuserviceprovider.h>
#include "cuactionfactoryservice.h"
#include "cudevicefactoryservice.h"
#include "cutangoactionfactoryi.h"
#include "cupollingservice.h"
#include "cutango-world.h"

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

class CumbiaTangoPrivate {
public:
    CumbiaTangoPrivate(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb) :
        th_factory_i(tfi), th_eve_b_factory(teb), updpo(CuDataUpdatePolicy::PollUpdateAlways) {}

    CuThreadFactoryImplI *th_factory_i;
    CuThreadsEventBridgeFactory_I *th_eve_b_factory;
    int updpo;
};

/** \brief CumbiaTango two parameters constructor
 *
 * The class constructor accepts a factory providing a thread implementation for cumbia (mainly CuThread)
 * and a <em>bridge</em> to forward events from the secondary threads to the main.
 *
 * @param tfi a CuThreadFactoryImplI implementation, mainly CuThreadFactoryImpl
 * @param teb a CuThreadsEventBridgeFactory_I implementation, for example CuThreadsEventBridgeFactory
 *        or cumbia-qtcontrols QThreadsEventBridgeFactory
 *
 * \par example
 * \code
 * CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
 * \endcode
 *
 */
CumbiaTango::CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb) {
    d = new CumbiaTangoPrivate(tfi, teb);
    m_init();
}

void CumbiaTango::m_init() {
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType), new CuActionFactoryService());
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType), new CuDeviceFactoryService());
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType), new CuPollingService());
}

CumbiaTango::~CumbiaTango()
{
    pdelete("~CumbiaTango %p", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(d->th_eve_b_factory)
        delete d->th_eve_b_factory;
    if(d->th_factory_i)
        delete d->th_factory_i;
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    af->cleanup();
    // do not call ApiUtil::cleanup to shutdown the ORB here
    // ORB could be shut down while some activities are still running
    // for example unsubscribing events in their threads
    // CuTangoWorld().orb_cleanup();
    delete d;
}

// invoked by CuTControlsReader::setSource
// f creates a new CuTangoActionI (CuTReader, CuTaDb, CuTConfiguration, CuTWriter) (cutangoactionfactories.cpp)
// a->start registers the related activity
void CumbiaTango::addAction(const TSource &source, CuDataListener *l, const CuTangoActionFactoryI& f) {
    bool isnew;
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI *a = af->registerAction(source, f, this, &isnew); // must be called with src = source.getName
    if(isnew)
        a->start();
    a->addDataListener(l);
}

void CumbiaTango::removeAction(const TSource &source, CuTangoActionI::Type t) {
    CuActionFactoryService * af = static_cast<CuActionFactoryService *>(getServiceProvider()
                                                                        ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
    af->unregisterAction(source, t);
}

/** \brief Removes a listener from the action with the given source and type
 *
 * @param source the source of the action
 * @param t the CuTangoActionI::Type type (Reader, Writer, ReaderConfig, WriterConfig, TaDb)
 * @param l the CuDataListener to be removed from the action identified by source and t
 *
 * \note Examples of CuDataListener are objects from the cumbia-qtcontrols module, such as QuLabel,
 *       QuTrendPlot, QuLed and the like.
 *
 * \note Examples of CuTangoActionI are CuTReader CuTWriter and CuTConfiguration
 *
 * \note An action is removed from cumbia when no more listeners reference it
 */
void CumbiaTango::unlinkListener(const string &source, CuTangoActionI::Type t, CuDataListener *l) {
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI * a = af->find(source, t);
    if(a)
        a->removeDataListener(l); /* when no more listeners, a stops */
}

CuTangoActionI *CumbiaTango::findAction(const std::string &source, CuTangoActionI::Type t) const {
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI* a = af->find(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaTango::getThreadFactoryImpl() const {
    return d->th_factory_i;
}

CuThreadsEventBridgeFactory_I *CumbiaTango::getThreadEventsBridgeFactory() const {
    return d->th_eve_b_factory;
}

/*! Suggest a policy for read updates
 *
 *  @param p a valid combination of values from the enum CuDataUpdatePolicy
 *  *OnPoll* options can be *or*-ed with *SkipFirstReadUpdate*
 *
 *  @see CuDataUpdatePolicy
 *  @see CuPollingActivity
 */
void CumbiaTango::setReadUpdatePolicy(int p) {
    d->updpo = p;
}

/*! Returns the update policy in use
 *
 * Default: CuDataUpdatePolicy::PollUpdateAlways
 */
int CumbiaTango::readUpdatePolicy() const {
    return d->updpo;
}

int CumbiaTango::getType() const {
    return CumbiaTangoType;
}
