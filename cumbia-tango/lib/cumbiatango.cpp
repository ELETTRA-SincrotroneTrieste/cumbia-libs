#include "cumbiatango.h"
#include <cumbia.h>
#include <culog.h>
#include <cumacros.h>
#include <cudatalistener.h>

#include <cuserviceprovider.h>
#include "cuactionfactoryservice.h"
#include "cudevicefactoryservice.h"
#include "cutangoactionfactoryi.h"
#include "cutango-world.h"
#include "cutreader.h"

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>

CumbiaTango::CumbiaTango()
{
    m_init();
}

CumbiaTango::CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb)
{
    m_threadsEventBridgeFactory = teb;
    m_threadFactoryImplI = tfi;
    m_init();
}

void CumbiaTango::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType), new CuActionFactoryService());
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType), new CuDeviceFactoryService());
}

void CumbiaTango::setThreadFactoryImpl( CuThreadFactoryImplI *tfi)
{
    m_threadFactoryImplI = tfi;
}

void CumbiaTango::setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb)
{
    m_threadsEventBridgeFactory = teb;
}

CumbiaTango::~CumbiaTango()
{
    printf("~CumbiaTango");
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(m_threadsEventBridgeFactory)
        delete m_threadsEventBridgeFactory;
    if(m_threadFactoryImplI)
        delete m_threadFactoryImplI;
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    af->deleteActions();
}

void CumbiaTango::addAction(const std::__cxx11::string &source, CuDataListener *l, const CuTangoActionFactoryI& f)
{
    CuTangoWorld w;
    if(w.source_valid(source))
    {
        CuActionFactoryService *af =
                static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));

        CuTangoActionI *a = af->findAction(source, f.getType());
        if(!a)
        {
            a = af->registerAction(source, f, this);
            a->start();
        }
        else
            printf("CumbiaTango.addAction: action %p already found for source \"%s\" and type %d thread 0x%lx\n",
                  a, source.c_str(), f.getType(), pthread_self());
        a->addDataListener(l);
    }
    else
        perr("CumbiaTango.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

/** \brief Removes a listener from the action identified by the given source name and type
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
 * @see CuTangoActionI::Type
 * @see CuDataListener
 * @see CuTReader::removeDataListener
 * @see CuTAttConfiguration::removeDataListener
 *
 * @see CuTangoActionI::removeDataListener
 */
void CumbiaTango::unlinkListener(const string &source, CuTangoActionI::Type t, CuDataListener *l)
{
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI* a = af->findAction(source, t);
    if(a)
        a->removeDataListener(l); /* when no more listeners, a stops itself */
}

CuTangoActionI *CumbiaTango::findAction(const std::string &source, CuTangoActionI::Type t) const
{
    CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
    CuTangoActionI* a = af->findAction(source, t);
    return a;
}

CuThreadFactoryImplI *CumbiaTango::getThreadFactoryImpl() const
{
    return m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaTango::getThreadEventsBridgeFactory() const
{
    return m_threadsEventBridgeFactory;
}

int CumbiaTango::getType() const
{
    return CumbiaTangoType;
}



