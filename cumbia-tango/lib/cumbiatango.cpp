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
#include "cutreader.h"

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

class CumbiaTangoPrivate {
public:
    CumbiaTangoPrivate(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb, CumbiaTango::Options o) :
        th_factory_i(tfi), th_eve_b_factory(teb), options(o) {}

    CuThreadFactoryImplI *th_factory_i;
    CuThreadsEventBridgeFactory_I *th_eve_b_factory;
    CumbiaTango::Options options;
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
    d = new CumbiaTangoPrivate(tfi, teb, CumbiaTango::OptionActionFactorySrvcThreadSafe);
    m_init();
}

CumbiaTango::CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb, Options o)
{
    d = new CumbiaTangoPrivate(tfi, teb, o);
    m_init();
}

void CumbiaTango::m_init()
{
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
    delete d;
}

void CumbiaTango::addAction(const TSource &source, CuDataListener *l, const CuTangoActionFactoryI& f) {
    CuTangoWorld w;
    const std::string& src = source.getName();
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI *a = af->find(src, f.getType());
    printf("CumbiaTango::addAction finds %p for %s type %d\n", a, src.c_str(), f.getType());
    if(!a) {
//        printf("CumbiaTango.addAction: new action created %s %p\n", src.c_str(), a);
        a = af->registerAction(src, f, this);
//        printf("CumbiaTango.addAction: starting action %s %p\n", src.c_str(), a);
        a->start();
    }
    else
        printf("CumbiaTango.addAction: action \e[0;33malready found\e[0m for src \e[0;33m%s\e[0m type %d %p\n", src.c_str(), f.getType(), a);
    a->addDataListener(l);
}

void CumbiaTango::removeAction(const string &source, CuTangoActionI::Type t) {
    CuActionFactoryService * af = static_cast<CuActionFactoryService *>(getServiceProvider()
                                                                        ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
    af->unregisterAction(source, t);
}

/** \brief Removes a listener from the action(s) with the given source name and type
 *
 * @param source the source of the action
 * @param t the CuTangoActionI::Type type (Reader, Writer, MultiReader, MultiWriter, AttConfig...)
 * @param l the CuDataListener to be removed from the action identified by source and t
 *
 * \note Examples of CuDataListener are objects from the cumbia-qtcontrols module, such as QuLabel,
 *       QuTrendPlot, QuLed and the like.
 *
 * \note Examples of CuTangoActionI are CuTReader CuTWriter and CuTAttConfiguration
 *
 * \note This call does not necessarily remove the CuActionI from cumbia. A CuActionI is removed when
 *       there are no linked CuDataListener anymore.
 *
 * \note There may be multiple actions with the given source and type. CuTangoActionI::removeDataListener(l)
 *       is called for each of them. Please see CuActionFactoryService::find for more details
 *
 * @see CuTangoActionI::Type
 * @see CuDataListener
 * @see CuTReader::removeDataListener
 * @see CuTAttConfiguration::removeDataListener
 *
 * @see CuTangoActionI::removeDataListener
 */
void CumbiaTango::unlinkListener(const string &source, CuTangoActionI::Type t, CuDataListener *l) {
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI * a = af->find(source, t);
    if(a)
        a->removeDataListener(l); /* when no more listeners, a stops itself */
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

int CumbiaTango::getType() const {
    return CumbiaTangoType;
}



