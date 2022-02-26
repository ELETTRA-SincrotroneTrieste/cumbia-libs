#include "cuactionfactoryservice.h"
#include "cutangoactionfactoryi.h"
#include "cutango-world.h"
#include "tdevice.h"
#include "tsource.h"
#include "cutangoactioni.h"
#include "cuactionfactoryservice_impls.h"


/*! \brief the class constructor
 *
 *  Instantiates a CuActionFactoryService
 *
 */
CuActionFactoryService::CuActionFactoryService() : CuServiceI() {
    impl = new CuActionFactoryServiceImpl;
}

CuActionFactoryService::~CuActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete impl;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuTangoActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *
 * @see findAction
 * @see unregisterAction
 */
CuTangoActionI* CuActionFactoryService::registerAction(const std::string& src,
                                                       const CuTangoActionFactoryI& f,
                                                       CumbiaTango* ct,
                                                       bool *isnew) {
    return impl->registerAction(src, f, ct, isnew);
}

/*! \brief find an action given the source name and the action type
 *
 * @param src the complete source (target) name (no wildcard)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 * @return the action with the given src and type.
 */
CuTangoActionI *CuActionFactoryService::find(const string &src, CuTangoActionI::Type at) {
    return impl->find(src, at);
}

/*! \brief return the number of registered actions
 *
 * @return the number of registered actions.
 *
 * Actions are added with registerAction and removed with unregisterAction
 */
size_t CuActionFactoryService::count() const {
    return impl->count();
}

/** \brief Unregister the action with the given source and type
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * \note
 * The action is <strong>not deleted</strong>.
 * CuTReader and CuTWriter auto delete themselves
 */
void CuActionFactoryService::unregisterAction(const string &src, CuTangoActionI::Type at) {
    return impl->unregisterAction(src, at);
}

/*! \brief all registered actions are <strong>deleted</strong> and removed from the service internal list.
 *
 * All references to the registered actions are removed from the service
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuActionFactoryService::cleanup() {
    impl->cleanup();
}

void CuActionFactoryService::reserve(unsigned chunks) {
    impl->reserve(chunks);
}

/*! \brief returns the service name
 *
 * @return the constant string "*ActionFactoryService" identifying this type of service.
 *
 * Implements CuTangoActionI::getName pure virtual method
 */
std::string CuActionFactoryService::getName() const {
    return "CuActionFactoryService";
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuActionFactoryServiceType = CuServices::User + 2)
 *
 * @see Type
 *
 * Implements CuTangoActionI::getType pure virtual method
 */
CuServices::Type CuActionFactoryService::getType() const {
    return static_cast<CuServices::Type >(CuActionFactoryServiceType);
}
