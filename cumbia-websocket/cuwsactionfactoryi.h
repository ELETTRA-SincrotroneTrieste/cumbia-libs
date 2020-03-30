#ifndef CUWSACTIONFACTORYI_H
#define CUWSACTIONFACTORYI_H

class CuWSClient;
class QString;

#include <string>
#include <cuwsactioni.h>

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
class CuWSActionFactoryI
{
public:
    CuWSActionFactoryI() {}

    /**
     * Pure virtual method that must be implemented by subclasses to return a valid CuTangoActionI
     * instance.
     *
     * @param source a string identifying a source to connect to
     * @param ct a pointer to a CumbiaWebSocket instance
     */
    virtual CuWSActionI *create(const std::string &s, CuWSClient *cli, const QString& http_addr) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuWSActionI::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
