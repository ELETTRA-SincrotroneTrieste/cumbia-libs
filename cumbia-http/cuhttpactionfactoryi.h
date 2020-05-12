#ifndef CUHTTPACTIONFACTORYI_H
#define CUHTTPACTIONFACTORYI_H

class QNetworkAccessManager;
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
     * Pure virtual method that must be implemented by subclasses to return a valid CuTangoActionI
     * instance.
     *
     * @param source a string identifying a source to connect to
     * @param ct a pointer to a CumbiaHttp instance
     */
    virtual CuHTTPActionA *create(const std::string &s, QNetworkAccessManager *nam, const QString& http_addr) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuHTTPActionA::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
