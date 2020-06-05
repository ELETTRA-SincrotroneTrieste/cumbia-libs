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

    /**
     * Pure virtual method that must be implemented by subclasses to return a valid CuHTTPActionA
     * instance.
     *
     */
    virtual CuHTTPActionA *create(const std::string &s,
                                  QNetworkAccessManager *nam,
                                  const QString& http_addr,
                                  CuHttpAuthManager *authman,
                                  CuHttpChannelReceiver* cr = nullptr) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuHTTPActionA::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
