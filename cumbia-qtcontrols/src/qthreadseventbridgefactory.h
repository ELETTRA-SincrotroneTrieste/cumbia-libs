#ifndef QTHREADSEVENTBRIDGEFACTORY_H
#define QTHREADSEVENTBRIDGEFACTORY_H

#include "cuthreadseventbridgefactory_i.h"

/** \brief a factory that instantiates a QThreadsEventBridge instance
 *
 * \ingroup core
 *
 * \note The usage of this *threads event bridge factory* is recommended in
 *       Qt applications. It works perfectly in combination with CuThread.
 *
 * The createEventBridge returns a QThreadsEventBridge, which in turn is an
 * implementation of CuThreadsEventBridge_I.
 *
 * A CuThreadsEventBridge is a *bridge* that takes an event and delivers it
 * to a secondary thread.
 *
 * See CuThreadsEventBridge_I
 *
 * @implements CuThreadsEventBridgeFactory_I
 *
 * \par Example
 *
 * \code
   CuData thread_tok("thread", "athread");
   Activity *a = new Activity(CuData("name", "activity"));
   m_cumbia->registerActivity(a, this, thread_tok,  // m_cumbia is a Cumbia* (e.g. defined as class attribute)
      CuThreadFactoryImpl(), // use CuThread
      QThreadsEventBridgeFactory()); // use Qt QApplication event loop
 * \endcode
 *
 * Please read <a href="../../cumbia/md_src_tutorial_cuactivity.html">Writing a simple activity </a> for a complete example
 */
class QThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
public:

    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider* sp = NULL) const;
};

#endif // QTHREADSEVENTBRIDGEFACTORY_H
