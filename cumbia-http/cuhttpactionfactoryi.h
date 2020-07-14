#ifndef CUHTTPACTIONFACTORYI_H
#define CUHTTPACTIONFACTORYI_H

class QNetworkAccessManager;
class CuHttpChannelReceiver;
class CuHttpAuthManager;
class QString;

#include <string>
#include <cuhttpactiona.h>

/** \brief Factory to instantiate CuTangoActionI implementations, such as readers, writers and
 *         attribute configuration actions.
 *
 * Notable implementations
 *
 * \li CuTangoReaderFactory
 * \li CuTangoWriterFactory
 * \li CuTangoAttConfFactory
 *
 */
class CuHTTPActionFactoryI
{
public:
    CuHTTPActionFactoryI() {}

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual std::string getMethod() const = 0;

    virtual CuHTTPActionFactoryI* clone() const = 0;

    virtual CuData options() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
