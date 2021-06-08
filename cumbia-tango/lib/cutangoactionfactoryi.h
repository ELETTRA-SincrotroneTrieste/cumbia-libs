#ifndef CUTANGOACTIONFACTORYI_H
#define CUTANGOACTIONFACTORYI_H

class CumbiaTango;

#include <string>
#include <cutangoactioni.h>

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
class CuTangoActionFactoryI
{
public:
    virtual ~CuTangoActionFactoryI() {}

    /**
     * Pure virtual method that must be implemented by subclasses to return a valid CuTangoActionI
     * instance.
     *
     * @param source a string identifying a source to connect to
     * @param ct a pointer to a CumbiaTango instance
     */
    virtual CuTangoActionI *create(const std::string& source, CumbiaTango *ct) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuTangoActionI::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
