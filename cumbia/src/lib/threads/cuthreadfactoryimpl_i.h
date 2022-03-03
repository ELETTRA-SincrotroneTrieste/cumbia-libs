#ifndef CUTHREADFACTORYIMPL_INTERFACE_H
#define CUTHREADFACTORYIMPL_INTERFACE_H

class CuThreadInterface;
class CuData;
class CuServiceProvider;
class CuThreadsEventBridge_I;

#include <cumacros.h>
#include <string>

/** \brief an interface that requires the implementation of a method to create a thread
 *
 * The implementors of this interface will provide the createThread method in order to
 * instantiate and return an implementation of the CuThreadInterface
 *
 * The CuThreadFactoryImpl is the cumbia base implementation of this interface.
 * It will return a new CuThread
 */
class CuThreadFactoryImplI
{
public:

    /*! \brief the class destructor
     *
     * provided to avoid compilers' warnings about virtual destructors
     */
    virtual ~CuThreadFactoryImplI() { pdelete("~CuThreadFactoryImpl %p", this); }

    /*! \brief instantiates and returns an instance of an implementation of the CuThreadInterface
     *
     * @param thread_token a token assigned to this thread. It will be used to decide whether
     *        new activities are run in the same thread or in a new one.
     *        See CuActivity::registerActivity and CuThread::isEquivalent for more details.
     *        See also md_src_tutorial_cuactivity for an example
     *
     * @param eventsBridge a pointer to an instance of an implementation of the
     *        CuThreadsEventBridge_I (see the documentation).
     *
     * @param serviceProvider a pointer to a CuServiceProvider
     *
     * @return an instance of an implementation of CuThreadInterface
     *
     * \par Threads event bridge
     *  A *thread event bridge* provides
     *  an interface to deliver events from a background thread to an event loop.
     *
     * Known implementations:
     *
     * \li CuThreadsEventBridge: *cumbia* bridge that relies on the CuEventLoopService
     * \li QThreadsEventBridge: Qt bridge that relies on QApplication and QApplication::postEvent.
     *     This is the first choice when developing Qt applications (cumbia-qtcontrols module)
     */
    virtual CuThreadInterface *createThread(const std::string& thread_token,
                                            CuThreadsEventBridge_I *eventsBridge,
                                            const CuServiceProvider *serviceProvider) const = 0;
};

#endif // CUTHREADFACTORYIMPL_H
